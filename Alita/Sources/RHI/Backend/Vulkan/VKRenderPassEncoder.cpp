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

#include <vector>
#include <array>
#include <vulkan/vulkan.h>

NS_RHI_BEGIN

VKRenderPassEncoder::VKRenderPassEncoder(VKDevice* device)
    : device_(device)
{
    vkDevice_ = device_->GetDevice();
    //
    //    RenderPassCreateInfo renderPassCreateInfo {
    //            .attachments = {
    //                    RHI::AttachmentDescription {
    //                            .format = RHI::Format::B8G8R8A8_UNORM,
    //                            .samples = RHI::SampleCountFlagBits::SAMPLE_COUNT_1_BIT,
    //                            .loadOp = RHI::AttachmentLoadOp::CLEAR,
    //                            .storeOp = RHI::AttachmentStoreOp::STORE,
    //                            .stencilLoadOp = RHI::AttachmentLoadOp::CLEAR,
    //                            .stencilStoreOp = RHI::AttachmentStoreOp::STORE,
    //                            .initialLayout = RHI::ImageLayout::UNDEFINED,
    //                            .finalLayout = RHI::ImageLayout::PRESENT_SRC_KHR
    //                    },
    //                    RHI::AttachmentDescription {
    //                            .format = RHI::Format::D24_UNORM_S8_UINT,
    //                            .samples = RHI::SampleCountFlagBits::SAMPLE_COUNT_1_BIT,
    //                            .loadOp = RHI::AttachmentLoadOp::CLEAR,
    //                            .storeOp = RHI::AttachmentStoreOp::STORE,
    //                            .stencilLoadOp = RHI::AttachmentLoadOp::CLEAR,
    //                            .stencilStoreOp = RHI::AttachmentStoreOp::STORE,
    //                            .initialLayout = RHI::ImageLayout::UNDEFINED,
    //                            .finalLayout = RHI::ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    //                    },
    //            },
    //            .subpasses = {
    //                    SubpassDescription {
    //                            .colorAttachments = {
    //                                    RHI::AttachmentReference {
    //                                            .attachment = 0,
    //                                            .layout = RHI::ImageLayout::COLOR_ATTACHMENT_OPTIMAL
    //                                    },
    //                            },
    //                            .depthStencilAttachment = {
    //                                    RHI::AttachmentReference {
    //                                            .attachment = 1,
    //                                            .layout = RHI::ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    //                                    },
    //                            },
    //                            .inputAttachments = {},
    //                            .pipelineBindPoint = RHI::PipelineBindPoint::GRAPHICS,
    //                            .preserveAttachments = {},
    //                            .resolveAttachments = {}
    //                    },
    //            },
    //            .dependencies = {}
    //    };
    //
    //    renderPass_ = (VKRenderPass*)device->CreateRenderPass(renderPassCreateInfo);
    //    RHI_SAFE_RETAIN(renderPass_);
}

VKRenderPassEncoder::~VKRenderPassEncoder()
{
    RHI_SAFE_RELEASE(renderPass_);
    if (vkFramebuffer_)
    {
        vkDestroyFramebuffer(vkDevice_, vkFramebuffer_, nullptr);
        vkFramebuffer_ = 0;
    }
}

void VKRenderPassEncoder::BeginPass(VkCommandBuffer vkCommandBuffer,
                                    const RenderPassDescriptor &descriptor)
{
    RHI_SAFE_RELEASE(renderPass_);
    RenderPassCacheQuery query;
    {
        std::uint32_t attachmentCount = 0;
        for (auto &colorState : descriptor.colorAttachments)
        {
            query.SetColor(attachmentCount, colorState.attachment->GetFormat(), colorState.loadOp);
            attachmentCount++;
        }
        
        if (descriptor.depthStencilAttachment.attachment)
        {
            auto dsFormat = descriptor.depthStencilAttachment.attachment->GetFormat();
            auto depthLoadOp = descriptor.depthStencilAttachment.depthLoadOp;
            auto stencilLoadOp = descriptor.depthStencilAttachment.stencilLoadOp;
            query.SetDepthStencil(dsFormat, depthLoadOp, stencilLoadOp);
        }
        
        renderPass_ = RHI_CAST(VKRenderPass*, device_->GetOrCreateRenderPass(query));
    }
    
    RHI_ASSERT(vkCommandBuffer != 0L);
    vkCommandBuffer_ = vkCommandBuffer;
    
    VkCommandBufferBeginInfo cmdBufferBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr
    };
    CALL_VK(vkBeginCommandBuffer(vkCommandBuffer_, &cmdBufferBeginInfo));
    
    // Create framebuffer
    if (vkFramebuffer_)
    {
        vkDestroyFramebuffer(vkDevice_, vkFramebuffer_, nullptr);
        vkFramebuffer_ = 0;
    }
    
    VKTextureView* colorAttachment = RHI_CAST(VKTextureView*,
                                              descriptor.colorAttachments[0].attachment);
    
    std::vector<VkImageView> attachments = {colorAttachment->GetNative()};
    
    VKTextureView* dsAttachment = RHI_CAST(VKTextureView*,
                                           descriptor.depthStencilAttachment.attachment);
    if (dsAttachment)
    {
        attachments.push_back(dsAttachment->GetNative());
    }
    
    const Extent3D &textureSize = colorAttachment->GetTextureSize();
    VkFramebufferCreateInfo framebufferInfo;
    {
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = nullptr;
        framebufferInfo.renderPass = renderPass_->GetNative();
        framebufferInfo.layers = 1;
        framebufferInfo.attachmentCount = (std::uint32_t)attachments.size();  // 2 if using depth
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = textureSize.width;
        framebufferInfo.height = textureSize.height;
    }
   
    CALL_VK(vkCreateFramebuffer(vkDevice_, &framebufferInfo, nullptr, &vkFramebuffer_));
    
    std::vector<VkClearValue> clearVals;
    for (auto &colorState : descriptor.colorAttachments)
    {
        VkClearValue clearValue{
            .color = {colorState.loadValue.r, colorState.loadValue.g, colorState.loadValue.b,
                      colorState.loadValue.a}
        };
        clearVals.push_back(clearValue);
    }
    {
        VkClearValue clearValue;
        {
            clearValue.depthStencil.depth = descriptor.depthStencilAttachment.depthLoadValue;
            clearValue.depthStencil.stencil = descriptor.depthStencilAttachment.stencilLoadValue;
        }
        clearVals.push_back(clearValue);
    }
    
    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = renderPass_->GetNative(),
        .framebuffer = vkFramebuffer_,
        .renderArea = {
            .offset = {.x = 0, .y = 0},
            .extent = {.width = textureSize.width, .height = textureSize.height}
        },
        .clearValueCount = (std::uint32_t) clearVals.size(),
        .pClearValues = clearVals.data()
    };
    
    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
}

void VKRenderPassEncoder::SetGraphicPipeline(const RenderPipeline* graphicPipeline)
{
    VkPipeline vkPipeline = RHI_CAST(const VKRenderPipeline*, graphicPipeline)->GetNative();
    vkCmdBindPipeline(vkCommandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
    
    graphicPipeline_ = graphicPipeline;
}

void VKRenderPassEncoder::SetIndexBuffer(const Buffer* buffer, std::uint32_t offset)
{
    VkBuffer vkBuffer = ((VKBuffer*) buffer)->GetNative();
    vkCmdBindIndexBuffer(vkCommandBuffer_, vkBuffer, offset, VK_INDEX_TYPE_UINT32);
}

void
VKRenderPassEncoder::SetVertexBuffer(const Buffer* buffer, std::uint32_t offset, std::uint32_t slot)
{
    VkBuffer vkBuffer = ((VKBuffer*) buffer)->GetNative();
    VkBuffer vertexBuffers[] = {vkBuffer};
    VkDeviceSize offsets[] = {offset};
    vkCmdBindVertexBuffers(vkCommandBuffer_, slot, 1, vertexBuffers, offsets);
}

void VKRenderPassEncoder::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount,
                               std::uint32_t firstVertex, std::uint32_t firstInstance)
{
    vkCmdDraw(vkCommandBuffer_, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VKRenderPassEncoder::Draw(std::uint32_t vertexCount, std::uint32_t firstVertex)
{
    this->Draw(vertexCount, 1, firstVertex, 0);
}

void VKRenderPassEncoder::DrawIndxed(std::uint32_t indexCount, std::uint32_t firstIndex)
{
    DrawIndexed(indexCount, 1, firstIndex, 0, 0);
}

void VKRenderPassEncoder::DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                                      std::uint32_t firstIndex, std::uint32_t baseVertex,
                                      std::uint32_t firstInstance)
{
    vkCmdDrawIndexed(vkCommandBuffer_, indexCount, instanceCount, firstIndex, baseVertex,
                     firstInstance);
}

void VKRenderPassEncoder::SetViewport(float x, float y, float width, float height, float minDepth,
                                      float maxDepth)
{
    VkViewport viewport = {x, y, width, height, minDepth, maxDepth};
    vkCmdSetViewport(vkCommandBuffer_, 0, 1, &viewport);
}

void VKRenderPassEncoder::SetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width,
                                         std::uint32_t height)
{
    VkRect2D scissor = {{x,     y},
                        {width, height}};
    vkCmdSetScissor(vkCommandBuffer_, 0, 1, &scissor);
}

void VKRenderPassEncoder::SetStencilReference(std::uint32_t reference)
{
    //    vkCmdSetStencilReference(vkCommandBuffer_, )
}

void VKRenderPassEncoder::SetBindGroup(std::uint32_t index, const BindGroup* bindGroup,
                                       const std::vector<std::uint32_t> &dynamicOffsets)
{
    RHI_ASSERT(graphicPipeline_);
    RHI_CAST(const VKBindGroup*, bindGroup)->WriteToGPU();
    auto vkGraphicPipeline = RHI_CAST(const VKRenderPipeline*, graphicPipeline_);
    RHI_CAST(const VKBindGroup*, bindGroup)->BindToCommandBuffer(index, vkCommandBuffer_,
                                                                 vkGraphicPipeline->GetPipelineLayout());
}

void VKRenderPassEncoder::EndPass()
{
    vkCmdEndRenderPass(vkCommandBuffer_);
    vkCommandBuffer_ = 0L;
    graphicPipeline_ = nullptr;
}

NS_RHI_END