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
#include "VKQuerySet.h"

#include <vector>
#include <array>
#include <vulkan/vulkan.h>

NS_GFX_BEGIN

VKRenderPassEncoder::VKRenderPassEncoder(DevicePtr device)
    : RenderPassEncoder(device)
{
}

void VKRenderPassEncoder::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    attachments_.clear();
    resolveTargets_.clear();
    
    GFX_DISPOSE_END();
}

VKRenderPassEncoder::~VKRenderPassEncoder()
{
    Dispose();
}

bool VKRenderPassEncoder::Init(CommandBufferPtr commandBuffer,
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
                attachments_.push_back(attachment.attachment);
                hasSwapchainImage_ =
                    hasSwapchainImage_ || attachment.attachment->IsSwapchainImage();
            }
            if (attachment.resolveTarget)
            {
                resolveTargets_.push_back(attachment.resolveTarget);
                hasSwapchainImage_ =
                    hasSwapchainImage_ || attachment.resolveTarget->IsSwapchainImage();
            }
        }
    }
    
    depthStencilAttachemnt_ = descriptor.depthStencilAttachment.attachment;
    commandBuffer_ = commandBuffer;
    
    std::vector<VkClearAttachment> clearAttachments;
    
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
                           GFX_CAST(const VKTextureView*, attachment.attachment)->GetSampleCount());
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

            auto view = GFX_CAST(const VKTextureView*, attachment.attachment);
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
        
        renderPass_= VKDEVICE()->GetOrCreateRenderPass(query);
    }
    
    {
        TextureViewPtr colorAttachment = nullptr;
        if (!descriptor.colorAttachments.empty())
        {
            colorAttachment = descriptor.colorAttachments[0].attachment;
        }
        auto dsAttachment = descriptor.depthStencilAttachment.attachment;
        
        Extent3D attachmentSize;
        if (colorAttachment)
        {
            attachmentSize = colorAttachment->GetTexture()->GetTextureSize();
        }
        else if (dsAttachment)
        {
            attachmentSize = dsAttachment->GetTexture()->GetTextureSize();
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
                query.attachments[attachmentCount++] = attachment.attachment;
            }
        }
        
        for (size_t i = 0; i < descriptor.colorAttachments.size(); ++i)
        {
            const auto &attachment = descriptor.colorAttachments[i];
            if (attachment.resolveTarget)
            {
                query.attachments[attachmentCount++] = attachment.resolveTarget;
            }
        }
        
        if (dsAttachment)
        {
            query.attachments[attachmentCount++] = dsAttachment;
        }
        
        GFX_PTR_ASSIGN(vkFramebuffer_, VKDEVICE()->GetOrCreateFramebuffer(query));
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
    
        GFX_CAST(VKCommandBuffer*, commandBuffer_)->BeginRenderPassInRecordingStage();

       /* GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdBeginRenderPass>(renderPass_,
                                                                  vkFramebuffer_,
                                                                  descriptor.occlusionQuerySet,
                                                                  count,
                                                                  colors.data(),
                                                                  depthValue,
                                                                  stencilValue);*/

        commandBuffer_->AddCmd([renderPass = renderPass_, framebuffer = vkFramebuffer_, occlusionQuerySet = descriptor.occlusionQuerySet,
            clearValueCount = count, clearColors = colors, depthValue, stencilValue](CommandBuffer* commandBuffer) {
            commandBuffer->BeginRenderPass(renderPass, framebuffer, occlusionQuerySet, clearValueCount, clearColors.data(), depthValue, stencilValue);
        });

        
        /*for (size_t i = 0; i < clearAttachments.size(); ++i)
        {
            GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdClearAttachment>(clearAttachments[i]);
        }*/
    }
    else
    {
        GFX_ASSERT(false);
    }
    
    return true;
}

void VKRenderPassEncoder::SetPipeline(RenderPipelinePtr graphicPipeline)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetGraphicPipeline>(graphicPipeline);

    commandBuffer_->AddCmd([graphicPipeline](CommandBuffer* commandBuffer) {
        commandBuffer->BindGraphicsPipeline(graphicPipeline);
    });
}

void VKRenderPassEncoder::SetIndexBuffer(BufferPtr buffer, std::uint32_t offset)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetIndexBuffer>(buffer, offset);

    commandBuffer_->AddCmd([buffer, offset](CommandBuffer* commandBuffer) {
        commandBuffer->SetIndexBuffer(buffer, offset);
        });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBuffer(buffer);
}

void
VKRenderPassEncoder::SetVertexBuffer(BufferPtr buffer, std::uint32_t offset, std::uint32_t slot)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetVertexBuffer>(buffer, offset, slot);

    commandBuffer_->AddCmd([buffer, offset, slot](CommandBuffer* commandBuffer) {
        commandBuffer->SetVertexBuffer(buffer, offset, slot);
        });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBuffer(buffer);
}

void VKRenderPassEncoder::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount,
                               std::uint32_t firstVertex, std::uint32_t firstInstance)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdDraw>(vertexCount,
    //                                               instanceCount,
    //                                               firstVertex,
    //                                               firstInstance);

    commandBuffer_->AddCmd([vertexCount, instanceCount, firstVertex, firstInstance](CommandBuffer* commandBuffer) {
        commandBuffer->Draw(vertexCount, instanceCount, firstVertex, firstInstance);
        });
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
    
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdDrawIndexed>(indexCount,
    //                                                      instanceCount,
    //                                                      firstIndex,
    //                                                      baseVertex,
    //                                                      firstInstance);

    commandBuffer_->AddCmd([indexCount, instanceCount, firstIndex, baseVertex, firstInstance](CommandBuffer* commandBuffer) {
        commandBuffer->DrawIndexed(indexCount, instanceCount, firstIndex, baseVertex, firstInstance);
        });
}

void VKRenderPassEncoder::DrawIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdDrawIndirect>(indirectBuffer, indirectOffset);

    commandBuffer_->AddCmd([indirectBuffer, indirectOffset](CommandBuffer* commandBuffer) {
        commandBuffer->DrawIndirect(indirectBuffer, indirectOffset);
        });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBuffer(indirectBuffer);
}

void VKRenderPassEncoder::DrawIndexedIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdDrawIndexedIndirect>(indirectBuffer, indirectOffset);

    commandBuffer_->AddCmd([indirectBuffer, indirectOffset](CommandBuffer* commandBuffer) {
        commandBuffer->DrawIndexedIndirect(indirectBuffer, indirectOffset);
        });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBuffer(indirectBuffer);
}

void VKRenderPassEncoder::SetViewport(float x, float y, float width, float height,
                                      float minDepth,
                                      float maxDepth)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetViewport>(x, y, width, height, minDepth, maxDepth);

    commandBuffer_->AddCmd([x, y, width, height, minDepth, maxDepth](CommandBuffer* commandBuffer) {
        commandBuffer->SetViewport(x, y, width, height, minDepth, maxDepth);
        });
}

void VKRenderPassEncoder::SetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width,
                                         std::uint32_t height)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetScissorRect>(x, y, width, height);

    commandBuffer_->AddCmd([x, y, width, height](CommandBuffer* commandBuffer) {
        commandBuffer->SetScissorRect(x, y, width, height);
        });
}

void VKRenderPassEncoder::SetStencilReference(std::uint32_t reference)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetStencilReference>(reference);

    commandBuffer_->AddCmd([reference](CommandBuffer* commandBuffer) {
        commandBuffer->SetStencilReference(reference);
        });
}

void VKRenderPassEncoder::SetBindGroup(std::uint32_t index, BindGroupPtr bindGroup,
                                       std::uint32_t dynamicOffsetCount,
                                       const std::uint32_t* dynamicOffsets)
{
   /* GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetBindGroup>(PipelineType::Graphic,
                                                           index,
                                                           bindGroup,
                                                           dynamicOffsetCount,
                                                           dynamicOffsets);*/

    std::vector< std::uint32_t> tmpOffsets(dynamicOffsetCount);
    if (dynamicOffsetCount > 0)
    {
        memcpy(tmpOffsets.data(), dynamicOffsets, sizeof(std::uint32_t) * dynamicOffsetCount);
    }

    commandBuffer_->AddCmd([index, bindGroup, dynamicOffsets = tmpOffsets](CommandBuffer* commandBuffer) {
        commandBuffer->SetBindGroupToGraphicPipeline(index,
            bindGroup,
            dynamicOffsets.size(),
            dynamicOffsets.data());
        });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBindGroup(bindGroup);
}

void VKRenderPassEncoder::PushDebugGroup(const std::string &groupLabel)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdPushDebugGroup>(groupLabel);

    commandBuffer_->AddCmd([groupLabel](CommandBuffer* commandBuffer) {
        commandBuffer->PushDebugGroup(groupLabel.c_str());
        });
}

void VKRenderPassEncoder::PopDebugGroup()
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdPopDebugGroup>();

    commandBuffer_->AddCmd([](CommandBuffer* commandBuffer) {
        commandBuffer->PopDebugGroup();
        });
}

void VKRenderPassEncoder::InsertDebugMarker(const std::string &markerLabel)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdInsertDebugMarker>(markerLabel);

    commandBuffer_->AddCmd([markerLabel](CommandBuffer* commandBuffer) {
        commandBuffer->InsertDebugMarker(markerLabel.c_str());
        });
}

void VKRenderPassEncoder::SetBlendColor(const Color &color)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetBlendColor>(color);

    commandBuffer_->AddCmd([color](CommandBuffer* commandBuffer) {
        commandBuffer->SetBlendColor(color);
        });
}

void VKRenderPassEncoder::BeginOcclusionQuery(std::uint32_t queryIndex)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdBeginOcclusionQuery>(queryIndex);

    commandBuffer_->AddCmd([queryIndex](CommandBuffer* commandBuffer) {
        commandBuffer->BeginOcclusionQuery(queryIndex);
        });
}

void VKRenderPassEncoder::EndOcclusionQuery(std::uint32_t queryIndex)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdEndOcclusionQuery>(queryIndex);

    commandBuffer_->AddCmd([queryIndex](CommandBuffer* commandBuffer) {
        commandBuffer->EndOcclusionQuery(queryIndex);
        });
}

void VKRenderPassEncoder::SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetDepthBias>(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);

    commandBuffer_->AddCmd([depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor](CommandBuffer* commandBuffer) {
        commandBuffer->SetDepthBias(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
        });
}

void VKRenderPassEncoder::EndPass()
{
    if (!commandBuffer_)
    {
        return;
    }
    
    GFX_CAST(VKCommandBuffer*, commandBuffer_)->EndRenderPassInRecordingStage();

    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdEndPass>();

    commandBuffer_->AddCmd([](CommandBuffer* commandBuffer) {
        commandBuffer->EndRenderPass();
        });
    
    if (hasSwapchainImage_)
    {
        GFX_CAST(VKCommandBuffer*, commandBuffer_)->MarkPresentSwapchain();
        hasSwapchainImage_ = false;
    }
    
    attachments_.clear();
    resolveTargets_.clear();
}

NS_GFX_END