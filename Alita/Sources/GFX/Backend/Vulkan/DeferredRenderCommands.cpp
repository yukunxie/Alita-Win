//
// Created by realxie on 2020-02-28.
//

#include "DeferredRenderCommands.h"

#include "VKCommandBuffer.h"
#include "VKBindGroup.h"
#include "VKRenderPipeline.h"
#include "VKComputePipeline.h"
#include "VKBuffer.h"
#include "VKRenderPass.h"
#include "VKFramebuffer.h"
#include "VKTexture.h"
#include "RenderThreading.h"
#include "VKSampler.h"
#include "VKTextureView.h"
#include "VKTypes.h"


NS_GFX_BEGIN

void DeferredCmdBeginCommandBuffer::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->BeginCommandBuffer();
}

void DeferredCmdBeginRenderPass::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->BeginRenderPass(renderPass_, framebuffer_, occlusionQuerySet_, clearValueCount_,
                                   clearColors_.data(),
                                   clearDepth_, clearStencil_);
}

void DeferredCmdClearAttachment::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->ClearAttachment(clearAttachment_);
}

void DeferredCmdSetGraphicPipeline::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->BindGraphicsPipeline(graphicPipeline_);
}

void DeferredCmdSetComputePipeline::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->BindComputePipeline(computePipeline_);
}

void DeferredCmdDispatch::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->Dispatch(x_, y_, z_);
}

void DeferredCmdDispatchIndirect::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->DispatchIndirect(indirectBuffer_, indirectOffset_);
}

void DeferredCmdDraw::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->Draw(vertexCount_, instanceCount_, firstVertex_, firstInstance_);
}

void DeferredCmdDrawIndexed::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->DrawIndexed(indexCount_, instanceCount_, firstIndex_, baseVertex_,
                               firstInstance_);
}

void DeferredCmdEndPass::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->EndRenderPass();
}

void DeferredCmdSetVertexBuffer::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->SetVertexBuffer(buffer_, offset_, slot_);
}

void DeferredCmdSetIndexBuffer::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->SetIndexBuffer(buffer_, offset_);
}

void DeferredCmdDrawIndirect::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->DrawIndirect(indirectBuffer_, indirectOffset_);
}

void DeferredCmdDrawIndexedIndirect::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->DrawIndexedIndirect(indirectBuffer_, indirectOffset_);
}

void DeferredCmdSetViewport::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->SetViewport(x_, y_, width_, height_, minDepth_, maxDepth_);
}

void DeferredCmdSetScissorRect::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->SetScissorRect(x_, y_, width_, height_);
}

void DeferredCmdSetStencilReference::Execute(VKCommandBuffer* commandBuffer)
{
    commandBuffer->SetStencilReference(reference_);
}

void DeferredCmdSetBindGroup::Execute(VKCommandBuffer* commandBuffer)
{
    std::uint32_t* pData = dynamicOffsets_.data();
    
    if (PipelineType::Graphic == pipelineType_)
    {
        commandBuffer->SetBindGroupToGraphicPipeline(index_,
                                                     bindGroup_,
                                                     dynamicOffsetCount_,
                                                     pData);
    }
    else
    {
        commandBuffer->SetBindGroupToComputePipeline(index_,
                                                     bindGroup_,
                                                     dynamicOffsetCount_,
                                                     pData);
    }
}

void DeferredCmdCopyBufferToTexture::Execute(VKCommandBuffer* commandBuffer_)
{
    auto &source = source_;
    auto &destination = destination_;
    auto &copySize = copySize_;
    
    auto buffer = GFX_CAST(VKBuffer*, source.buffer);
    auto texture = GFX_CAST(VKTexture*, destination.texture);
    
    
    texture->TransToCopyDstImageLayout(commandBuffer_);
    
    VkBufferImageCopy bufferImageCopy;
    {
        bufferImageCopy.bufferOffset = source.offset;
        // bufferRowLength measured by pixel.
        bufferImageCopy.bufferRowLength =
            source.bytesPerRow / GetTextureFormatPixelSize(texture->GetFormat());
        bufferImageCopy.bufferImageHeight = source.rowsPerImage;
        bufferImageCopy.imageSubresource = {
            VKTexture::GetVkImageAspectFlags(texture->GetNativeFormat()), 
            destination.mipLevel,
            (std::uint32_t)destination.baseArrayLayer, 
            destination.arrayLayerCount
        };
        bufferImageCopy.imageOffset = {destination.origin.x, destination.origin.y, destination.origin.z};
        bufferImageCopy.imageExtent = {copySize.width, copySize.height, copySize.depth};
    }
    vkCmdCopyBufferToImage(commandBuffer_->GetNative(),
                             buffer->GetNative(),
                             texture->GetNative(),
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             1,
                             &bufferImageCopy);
}

void DeferredCmdCopyTextureToBuffer::Execute(gfx::VKCommandBuffer* commandBuffer_)
{
    auto &source = source_;
    auto &destination = destination_;
    auto &copySize = copySize_;
    
    VKTexture* texture = GFX_CAST(VKTexture*, source.texture);
    VKBuffer* buffer = GFX_CAST(VKBuffer*, destination.buffer);
    
    // Auto trans image layout to valid image layout.
    texture->TransToCopySrcImageLayout(commandBuffer_);
    
    VkBufferImageCopy region;
    region.imageOffset = {source.origin.x, source.origin.y, 0};
    region.imageExtent = {copySize.width, copySize.height, copySize.depth};
    region.imageSubresource = {VKTexture::GetVkImageAspectFlags(texture->GetNativeFormat()),
                               source.mipLevel, (std::uint32_t)source.origin.z, 1};
    region.bufferOffset = destination.offset;
    region.bufferRowLength = destination.bytesPerRow / GetTextureFormatPixelSize(texture->GetFormat());
    region.bufferImageHeight = destination.rowsPerImage;
    
    vkCmdCopyImageToBuffer(commandBuffer_->GetNative(),
                             texture->GetNative(),
                             VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                             buffer->GetNative(),
                             1,
                             &region);
}

void DeferredCmdCopyTextureToTexture::Execute(gfx::VKCommandBuffer* commandBuffer_)
{
    auto &source = source_;
    auto &destination = destination_;
    auto &copySize = copySize_;
    
    auto vkSrcTexture = GFX_CAST(VKTexture*, source.texture);
    auto vkDstTexture = GFX_CAST(VKTexture*, destination.texture);
    
    vkSrcTexture->TransToCopySrcImageLayout(commandBuffer_);
    vkDstTexture->TransToCopyDstImageLayout(commandBuffer_);
    
    // 这里使用了相对比较tricky的转换方式，依赖于1D,2D,3D的定义方式
    int32_t srcOffestZ = 0;
    int32_t dstOffestZ = 0;
    
    // 3D纹理暂未处理
    GFX_ASSERT(!source.texture->Is3DImage());
    GFX_ASSERT(!destination.texture->Is3DImage());
    
    VkImageCopy region;
    region.extent = {copySize.width, copySize.height, copySize.depth};
    if (source.texture->GetImageType() == destination.texture->GetImageType() && !destination.texture->Is3DImage())
    {
        region.extent.depth = 1;
    }
    
    region.srcOffset = {source.origin.x, source.origin.y, srcOffestZ};
    region.srcSubresource = {VKTexture::GetVkImageAspectFlags(vkSrcTexture->GetNativeFormat()),
                             source.mipLevel, (std::uint32_t)source.origin.z, 1};
    region.dstOffset = {destination.origin.x, destination.origin.y, dstOffestZ};
    region.dstSubresource = {VKTexture::GetVkImageAspectFlags(vkDstTexture->GetNativeFormat()),
                             destination.mipLevel, (std::uint32_t)destination.origin.z, 1};
    
    vkCmdCopyImage(commandBuffer_->GetNative(),
                     vkSrcTexture->GetNative(),
                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                     vkDstTexture->GetNative(),
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     1,
                     &region);
}

void DeferredCmdCopyBufferToBuffer::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    VkBuffer srcBuffer = GFX_CAST(VKBuffer*, source_)->GetNative();
    VkBuffer dstBuffer = GFX_CAST(VKBuffer*, destination_)->GetNative();
    VkBufferCopy region;
    {
        region.size = size_;
        region.srcOffset = sourceOffset_;
        region.dstOffset = destinationOffset_;
    }
    vkCmdCopyBuffer(commandBuffer->GetNative(), srcBuffer, dstBuffer, 1, &region);
}

void DeferredCmdSetBlendColor::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->SetBlendColor(color_);
}

void DeferredCmdSetDepthBias::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->SetDepthBias(depthBiasConstantFactor_, depthBiasClamp_, depthBiasSlopeFactor_);
}

void DeferredCmdPushDebugGroup::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->PushDebugGroup(strings_.data());
}

void DeferredCmdPopDebugGroup::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->PopDebugGroup();
}

void DeferredCmdInsertDebugMarker::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->PushDebugGroup(strings_.data());
}

void DeferredCmdExecuteBundle::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    renderBundle_->Execute(commandBuffer);
}

void DeferredCmdBeginOcclusionQuery::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->BeginOcclusionQuery(queryIndex_);
}

void DeferredCmdEndOcclusionQuery::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->EndOcclusionQuery(queryIndex_);
}

void DeferredCmdResolveQuerySet::Execute(gfx::VKCommandBuffer* commandBuffer)
{
    commandBuffer->ResolveQuerySet(querySet_, queryFirstIndex_, queryCount_, dstBuffer_,
                                   dstOffset_);
}

void DeferredCmdPipelineBarrier::Execute(VKCommandBuffer* commandBuffer)
{
    TextureFormat textureFormat = texture_->GetFormat();
    
    VkPipelineStageFlags srcStages = VulkanPipelineStage(srcUsageFlags_, textureFormat);
    VkPipelineStageFlags dstStages = VulkanPipelineStage(dstUsageFlags_, textureFormat);
    
    VkImageMemoryBarrier barrier;
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.srcAccessMask = VulkanAccessFlags(srcUsageFlags_, textureFormat);
    barrier.dstAccessMask = VulkanAccessFlags(dstUsageFlags_, textureFormat);
    barrier.oldLayout = GetVulkanImageLayout(srcUsageFlags_, textureFormat);
    barrier.newLayout = GetVulkanImageLayout(dstUsageFlags_, textureFormat);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = GFX_CAST(VKTexture*, texture_)->GetNative();
    
    // This transitions the whole resource but assumes it is a 2D texture
    barrier.subresourceRange.aspectMask = VulkanAspectMask(textureFormat);
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = texture_->GetMipLevelCount();
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = texture_->GetArrayLayerCount();
    
    auto vkCommandBuffer = commandBuffer->GetNative();
    vkCmdPipelineBarrier(vkCommandBuffer, srcStages, dstStages, 0, 0, NULL, 0, NULL, 1, &barrier);
}


NS_GFX_END
