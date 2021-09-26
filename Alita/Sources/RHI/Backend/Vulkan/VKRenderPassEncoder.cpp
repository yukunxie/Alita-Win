//
// Created by realxie on 2019-10-18.
//

#include "VKRenderPassEncoder.h"
#include "VKTypes.h"
#include "VKRenderPipeline.h"
#include "VKBuffer.h"
#include "VKTextureView.h"
#include "VKBindGroup.h"
#include "VKRenderPass.h"
#include "VKCommandBuffer.h"
#include "VKRenderBundleEncoder.h"
#include "VKQuerySet.h"

#include <vector>
#include <array>
#include <vulkan/vulkan.h>

NS_RHI_BEGIN

VKRenderPassEncoder::VKRenderPassEncoder(VKDevice* device)
    : RenderPassEncoder(device)
{
}

void VKRenderPassEncoder::Dispose()
{
    RHI_DISPOSE_BEGIN();
    
    attachments_.clear();
    resolveTargets_.clear();
    RHI_SAFE_RELEASE(commandBuffer_);
    RHI_SAFE_RELEASE(depthStencilAttachemnt_);
    RHI_SAFE_RELEASE(renderPass_);
    RHI_SAFE_RELEASE(vkFramebuffer_);
    
    RHI_DISPOSE_END();
}

VKRenderPassEncoder::~VKRenderPassEncoder()
{
    Dispose();
}

bool VKRenderPassEncoder::Init(VKCommandBuffer* commandBuffer,
                                    const RenderPassDescriptor &descriptor)
{
    hasSwapchainImage_ = false;
    
    // Make a reference to attachment.
    {
        attachments_.clear();
        resolveTargets_.clear();
        for (size_t i = 0; i < descriptor.colorAttachments.size(); ++i)
        {
            const auto &attachment = descriptor.colorAttachments[i];
            if (attachment.attachment)
            {
                attachments_.pushBack(attachment.attachment);
                hasSwapchainImage_ =
                    hasSwapchainImage_ || attachment.attachment->IsSwapchainImage();
            }
            if (attachment.resolveTarget)
            {
                resolveTargets_.pushBack(attachment.resolveTarget);
                hasSwapchainImage_ =
                    hasSwapchainImage_ || attachment.resolveTarget->IsSwapchainImage();
            }
        }
    }
    
    RHI_PTR_ASSIGN(depthStencilAttachemnt_, descriptor.depthStencilAttachment.attachment);
    RHI_PTR_ASSIGN(commandBuffer_, commandBuffer);
    
    TurboVector<VkClearAttachment> clearAttachments;
    
    RenderPassCacheQuery query;
    {
        std::uint32_t attachmentCount = 0;
        for (size_t i = 0; i < descriptor.colorAttachments.size(); ++i)
        {
            const auto &attachment = descriptor.colorAttachments[i];
            query.SetColor(attachmentCount,
                           attachment.attachment->GetFormat(),
                           attachment.loadOp,
                           attachment.storeOp,
                           attachment.resolveTarget != nullptr,
                           RHI_CAST(const VKTextureView*, attachment.attachment)->GetSampleCount());
            if (attachment.loadOp == LoadOp::CLEAR)
            {
                VkClearAttachment clearAttachment;
                clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                clearAttachment.colorAttachment = attachmentCount;
                const auto &loadValue = attachment.loadValue;
                clearAttachment.clearValue.color = {loadValue.r, loadValue.g, loadValue.b,
                                                    loadValue.a};
                clearAttachments.push_back(clearAttachment);
            }

            auto view = RHI_CAST(const VKTextureView*, attachment.attachment);
            query.bIsSwapchainTextures[i] = view->GetTexture()->IsSwapchainImage();

            attachmentCount++;
        }
        
        if (descriptor.depthStencilAttachment.attachment)
        {
            const auto &dsAttachment = descriptor.depthStencilAttachment;
            
            query.SetDepthStencil(dsAttachment.attachment->GetFormat(),
                                  dsAttachment.depthLoadOp,
                                  dsAttachment.depthStoreOp,
                                  dsAttachment.stencilLoadOp,
                                  dsAttachment.stencilStoreOp);
            
            VkClearAttachment clearAttachment;
            clearAttachment.aspectMask = 0;
            clearAttachment.colorAttachment = attachmentCount;
            clearAttachment.clearValue.depthStencil.depth = descriptor.depthStencilAttachment.depthLoadValue;
            clearAttachment.clearValue.depthStencil.stencil = descriptor.depthStencilAttachment.stencilLoadValue;
            
            if (dsAttachment.depthLoadOp == LoadOp::CLEAR)
            {
                clearAttachment.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
            }
            
            if (dsAttachment.stencilLoadOp == LoadOp::CLEAR)
            {
                clearAttachment.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
            
            clearAttachments.push_back(clearAttachment);
        }
        
        RHI_PTR_ASSIGN(renderPass_, VKDEVICE()->GetOrCreateRenderPass(query));
    }
    
    {
        auto colorAttachment = RHI_CAST(const VKTextureView*,
                                                  descriptor.colorAttachments[0].attachment);
        
        auto dsAttachment = RHI_CAST(const VKTextureView*,
                                               descriptor.depthStencilAttachment.attachment);
        
        Extent3D attachmentSize;
        if (colorAttachment)
        {
            attachmentSize = colorAttachment->GetTextureSize();
        }
        else if (dsAttachment)
        {
            attachmentSize = dsAttachment->GetTextureSize();
        }
        else
        {
            attachmentSize.width = VKDEVICE()->GetDefaultViewport().width;
            attachmentSize.height = VKDEVICE()->GetDefaultViewport().height;
            attachmentSize.depth = 1;
        }
        
        FramebufferCacheQuery query;
        query.layers = 1;
        query.renderPass = renderPass_;
        query.width = attachmentSize.width;
        query.height = attachmentSize.height;
        
        std::uint32_t attachmentCount = 0;
        for (size_t i = 0; i < descriptor.colorAttachments.size(); ++i)
        {
            const auto &attachment = descriptor.colorAttachments[i];
            if (attachment.attachment)
            {
                query.attachments[attachmentCount++] = RHI_CAST(const VKTextureView*,
                                                                attachment.attachment);
            }
        }
        
        for (size_t i = 0; i < descriptor.colorAttachments.size(); ++i)
        {
            const auto &attachment = descriptor.colorAttachments[i];
            if (attachment.resolveTarget)
            {
                query.attachments[attachmentCount++] = RHI_CAST(const VKTextureView*,
                                                                attachment.resolveTarget);
            }
        }
        
        if (dsAttachment)
        {
            query.attachments[attachmentCount++] = dsAttachment;
        }
        
        RHI_PTR_ASSIGN(vkFramebuffer_, VKDEVICE()->GetOrCreateFramebuffer(query));
    }
    
    if (descriptor.colorAttachments.size() <= kMaxColorAttachments)
    {
        std::array<Color, kMaxColorAttachments> colors;
        std::uint32_t count = 0;
        for (size_t i = 0; i < descriptor.colorAttachments.size(); ++i)
        {
            const auto &attachment = descriptor.colorAttachments[i];
            colors[count++] = attachment.loadValue;
        }
        
        float depthValue = descriptor.depthStencilAttachment.depthLoadValue;
        std::uint32_t stencilValue = descriptor.depthStencilAttachment.stencilLoadValue;
    
        commandBuffer_->BeginRenderPassInRecordingStage();
        commandBuffer_->RecordCommand<DeferredCmdBeginRenderPass>(renderPass_,
                                                                  vkFramebuffer_,
                                                                  descriptor.occlusionQuerySet,
                                                                  count,
                                                                  colors.data(),
                                                                  depthValue,
                                                                  stencilValue);
        commandBuffer_->AddBindingObject(renderPass_);
        commandBuffer_->AddBindingObject(vkFramebuffer_);
        commandBuffer_->AddBindingObject(descriptor.occlusionQuerySet);
        
        for (size_t i = 0; i < clearAttachments.size(); ++i)
        {
            commandBuffer_->RecordCommand<DeferredCmdClearAttachment>(clearAttachments[i]);
        }
    }
    else
    {
        RHI_ASSERT(false);
    }
    
    return true;
}

void VKRenderPassEncoder::SetPipeline(RenderPipeline* graphicPipeline)
{
    commandBuffer_->RecordCommand<DeferredCmdSetGraphicPipeline>(graphicPipeline);
    commandBuffer_->AddBindingObject(graphicPipeline);
}

void VKRenderPassEncoder::SetIndexBuffer(Buffer* buffer, std::uint32_t offset)
{
    commandBuffer_->RecordCommand<DeferredCmdSetIndexBuffer>(buffer, offset);
    commandBuffer_->AddBindingObject(buffer);
}

void
VKRenderPassEncoder::SetVertexBuffer(Buffer* buffer, std::uint32_t offset, std::uint32_t slot)
{
    commandBuffer_->RecordCommand<DeferredCmdSetVertexBuffer>(buffer, offset, slot);
    commandBuffer_->AddBindingObject(buffer);
}

void VKRenderPassEncoder::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount,
                               std::uint32_t firstVertex, std::uint32_t firstInstance)
{
    commandBuffer_->RecordCommand<DeferredCmdDraw>(vertexCount,
                                                   instanceCount,
                                                   firstVertex,
                                                   firstInstance);
}

void VKRenderPassEncoder::Draw(std::uint32_t vertexCount, std::uint32_t firstVertex)
{
    Draw(vertexCount, 1, firstVertex, 0);
}

void VKRenderPassEncoder::DrawIndxed(std::uint32_t indexCount, std::uint32_t firstIndex)
{
    DrawIndexed(indexCount, 1, firstIndex, 0, 0);
}

void VKRenderPassEncoder::DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                                      std::uint32_t firstIndex, int32_t baseVertex,
                                      std::uint32_t firstInstance)
{
    
    commandBuffer_->RecordCommand<DeferredCmdDrawIndexed>(indexCount,
                                                          instanceCount,
                                                          firstIndex,
                                                          baseVertex,
                                                          firstInstance);
}

void VKRenderPassEncoder::DrawIndirect(Buffer* indirectBuffer, BufferSize indirectOffset)
{
    commandBuffer_->RecordCommand<DeferredCmdDrawIndirect>(indirectBuffer, indirectOffset);
    commandBuffer_->AddBindingObject(indirectBuffer);
}

void VKRenderPassEncoder::DrawIndexedIndirect(Buffer* indirectBuffer, BufferSize indirectOffset)
{
    commandBuffer_->RecordCommand<DeferredCmdDrawIndexedIndirect>(indirectBuffer, indirectOffset);
    commandBuffer_->AddBindingObject(indirectBuffer);
}

void VKRenderPassEncoder::SetViewport(float x, float y, float width, float height,
                                      float minDepth,
                                      float maxDepth)
{
    commandBuffer_->RecordCommand<DeferredCmdSetViewport>(x, y, width, height, minDepth, maxDepth);
}

void VKRenderPassEncoder::SetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width,
                                         std::uint32_t height)
{
    commandBuffer_->RecordCommand<DeferredCmdSetScissorRect>(x, y, width, height);
}

void VKRenderPassEncoder::SetStencilReference(std::uint32_t reference)
{
    commandBuffer_->RecordCommand<DeferredCmdSetStencilReference>(reference);
}

void VKRenderPassEncoder::SetBindGroup(std::uint32_t index, BindGroup* bindGroup,
                                       std::uint32_t dynamicOffsetCount,
                                       const std::uint32_t* dynamicOffsets)
{
    commandBuffer_->RecordCommand<DeferredCmdSetBindGroup>(PipelineType::Graphic,
                                                           index,
                                                           bindGroup,
                                                           dynamicOffsetCount,
                                                           dynamicOffsets);
    commandBuffer_->AddBindingObject(bindGroup);
}

void VKRenderPassEncoder::PushDebugGroup(const std::string &groupLabel)
{
    commandBuffer_->RecordCommand<DeferredCmdPushDebugGroup>(groupLabel);
}

void VKRenderPassEncoder::PopDebugGroup()
{
    commandBuffer_->RecordCommand<DeferredCmdPopDebugGroup>();
}

void VKRenderPassEncoder::InsertDebugMarker(const std::string &markerLabel)
{
    commandBuffer_->RecordCommand<DeferredCmdInsertDebugMarker>(markerLabel);
}

void VKRenderPassEncoder::SetBlendColor(const Color &color)
{
    commandBuffer_->RecordCommand<DeferredCmdSetBlendColor>(color);
}

void VKRenderPassEncoder::ExecuteBundles(std::uint32_t count, RenderBundle** bundles)
{
    for (size_t i = 0; i < count; ++i)
    {
        commandBuffer_->RecordCommand<DeferredCmdExecuteBundle>(bundles[i]);
        commandBuffer_->AddBindingObject(bundles[i]);
    }
}

void VKRenderPassEncoder::BeginOcclusionQuery(std::uint32_t queryIndex)
{
    commandBuffer_->RecordCommand<DeferredCmdBeginOcclusionQuery>(queryIndex);
}

void VKRenderPassEncoder::EndOcclusionQuery(std::uint32_t queryIndex)
{
    commandBuffer_->RecordCommand<DeferredCmdEndOcclusionQuery>(queryIndex);
}

void VKRenderPassEncoder::SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    commandBuffer_->RecordCommand<DeferredCmdSetDepthBias>(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void VKRenderPassEncoder::EndPass()
{
    if (!commandBuffer_)
    {
        return;
    }
    
    commandBuffer_->EndRenderPassInRecordingStage();
    commandBuffer_->RecordCommand<DeferredCmdEndPass>();
    
    if (hasSwapchainImage_)
    {
        commandBuffer_->MarkPresentSwapchain();
        hasSwapchainImage_ = false;
    }
    
    attachments_.clear();
    resolveTargets_.clear();
    RHI_SAFE_RELEASE(commandBuffer_);
    RHI_SAFE_RELEASE(depthStencilAttachemnt_);
    RHI_SAFE_RELEASE(renderPass_);
    RHI_SAFE_RELEASE(vkFramebuffer_);
}

NS_RHI_END