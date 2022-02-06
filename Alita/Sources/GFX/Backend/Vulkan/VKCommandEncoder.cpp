//
// Created by realxie on 2019-10-14.
//

#include "VKCommandEncoder.h"
#include "VKCommandBuffer.h"
#include "VKRenderPassEncoder.h"
#include "VKComputePassEncoder.h"
#include "VKBuffer.h"
#include "VKTexture.h"

extern int g_test_released;

NS_GFX_BEGIN

VKCommandEncoder::VKCommandEncoder(const DevicePtr& device)
    : CommandEncoder(device)
{
    commandBuffer_ = VKDEVICE()->CreateCommandBuffer();
}

void VKCommandEncoder::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    commandBuffer_ = nullptr;
    renderPassEncoder_ = nullptr;
    computePassEncoder_ = nullptr;
    
    GFX_DISPOSE_END();
}

VKCommandEncoder::~VKCommandEncoder()
{
    if (g_test_released)
    {
        LOGW("xxxxxxxxxxxxxxxxxxxxx ~VKCommandEncoder");
    }
    
    Dispose();
}

bool VKCommandEncoder::Init()
{
    return true;
}

void VKCommandEncoder::Reset()
{
    commandBuffer_ = nullptr;
    renderPassEncoder_ = nullptr;
    computePassEncoder_ = nullptr;
    
    commandBuffer_ = VKDEVICE()->CreateCommandBuffer();
}

RenderPassEncoderPtr VKCommandEncoder::BeginRenderPass(const RenderPassDescriptor &descriptor)
{
    if (renderPassEncoder_)
    {
        renderPassEncoder_->EndPass();
    }
    
    auto newRenderPassEncoder = VKDEVICE()->CreateObject<RenderPassEncoderPtr, VKRenderPassEncoder>(commandBuffer_, descriptor);
    renderPassEncoder_ = newRenderPassEncoder;
    
    return renderPassEncoder_;
}

ComputePassEncoderPtr VKCommandEncoder::BeginComputePass(const ComputePassDescriptor &descriptor)
{
    if (computePassEncoder_)
    {
        computePassEncoder_->EndPass();
    }
    
    auto newComputePassEncoder = VKDEVICE()->CreateObject<ComputePassEncoderPtr, VKComputePassEncoder>(commandBuffer_, descriptor);
    
    return computePassEncoder_;
}

CommandBufferPtr VKCommandEncoder::Finish(const CommandBufferDescriptor &descriptor)
{
    return commandBuffer_;
}

void VKCommandEncoder::CopyBufferToBuffer(
    BufferPtr source,
    BufferSize sourceOffset,
    BufferPtr destination,
    BufferSize destinationOffset,
    BufferSize size)
{
    /*GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdCopyBufferToBuffer>(source, sourceOffset, destination,
                                                                 destinationOffset, size);*/

    commandBuffer_->AddCmd([source_ = source, sourceOffset_ = sourceOffset, destination_ = destination, destinationOffset_ = destinationOffset, size_ = size](CommandBuffer* cmdBuff) {
        auto commandBuffer = (VKCommandBuffer*) cmdBuff;
        VkBuffer srcBuffer = GFX_CAST(VKBuffer*, source_)->GetNative();
        VkBuffer dstBuffer = GFX_CAST(VKBuffer*, destination_)->GetNative();
        VkBufferCopy region;
        {
            region.size = size_;
            region.srcOffset = sourceOffset_;
            region.dstOffset = destinationOffset_;
        }
        vkCmdCopyBuffer(commandBuffer->GetNative(), srcBuffer, dstBuffer, 1, &region);
        });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBuffer(source);
    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBuffer(destination);
}

void VKCommandEncoder::CopyBufferToTexture(
    BufferCopyView &source,
    TextureCopyView &destination,
    Extent3D &copySize)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdCopyBufferToTexture>(source, destination, copySize);

    commandBuffer_->AddCmd([source = source, destination = destination, copySize = copySize](CommandBuffer* cmdBuff) {
        auto commandBuffer_ = (VKCommandBuffer*)cmdBuff;
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
                VKTexture::GetVkImageAspectFlags(texture->GetNativeFormat()), destination.mipLevel,
                (std::uint32_t)destination.origin.z,1 };
            bufferImageCopy.imageOffset = { destination.origin.x, destination.origin.y, 0 };
            bufferImageCopy.imageExtent = { copySize.width, copySize.height, copySize.depth };
        }
        vkCmdCopyBufferToImage(commandBuffer_->GetNative(),
            buffer->GetNative(),
            texture->GetNative(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &bufferImageCopy);
        });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBuffer(source.buffer);
}

void VKCommandEncoder::CopyTextureToBuffer(
    TextureCopyView &source,
    BufferCopyView &destination,
    Extent3D &copySize)
{
    VKTexture* texture = GFX_CAST(VKTexture*, source.texture);
    if (false == texture->TestTextureUsage(TextureUsage::COPY_SRC))
    {
        LOGE("CopyTextureToBuffer: the source texture must has COPY_SRC bit");
        return;
    }
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdCopyTextureToBuffer>(source, destination, copySize);

    commandBuffer_->AddCmd([source = source, destination = destination, copySize = copySize](CommandBuffer* cmdBuff) {
        auto commandBuffer_ = (VKCommandBuffer*)cmdBuff;
        VKTexture* texture = GFX_CAST(VKTexture*, source.texture);
        VKBuffer* buffer = GFX_CAST(VKBuffer*, destination.buffer);

        // Auto trans image layout to valid image layout.
        texture->TransToCopySrcImageLayout(commandBuffer_);

        VkBufferImageCopy region;
        region.imageOffset = { source.origin.x, source.origin.y, 0 };
        region.imageExtent = { copySize.width, copySize.height, copySize.depth };
        region.imageSubresource = { VKTexture::GetVkImageAspectFlags(texture->GetNativeFormat()),
                                   source.mipLevel, (std::uint32_t)source.origin.z, 1 };
        region.bufferOffset = destination.offset;
        region.bufferRowLength = destination.bytesPerRow / GetTextureFormatPixelSize(texture->GetFormat());
        region.bufferImageHeight = destination.rowsPerImage;

        vkCmdCopyImageToBuffer(commandBuffer_->GetNative(),
            texture->GetNative(),
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            buffer->GetNative(),
            1,
            &region);
        });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBuffer(destination.buffer);
}

void VKCommandEncoder::CopyTextureToTexture(
    TextureCopyView &source,
    TextureCopyView &destination,
    Extent3D &copySize)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdCopyTextureToTexture>(source, destination, copySize);

    commandBuffer_->AddCmd([source = source, destination = destination, copySize = copySize](CommandBuffer* cmdBuff) {
        auto commandBuffer_ = (VKCommandBuffer*)cmdBuff;
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
        region.extent = { copySize.width, copySize.height, copySize.depth };
        if (source.texture->GetImageType() == destination.texture->GetImageType() && !destination.texture->Is3DImage())
        {
            region.extent.depth = 1;
        }

        region.srcOffset = { source.origin.x, source.origin.y, srcOffestZ };
        region.srcSubresource = { VKTexture::GetVkImageAspectFlags(vkSrcTexture->GetNativeFormat()),
                                 source.mipLevel, (std::uint32_t)source.origin.z, 1 };
        region.dstOffset = { destination.origin.x, destination.origin.y, dstOffestZ };
        region.dstSubresource = { VKTexture::GetVkImageAspectFlags(vkDstTexture->GetNativeFormat()),
                                 destination.mipLevel, (std::uint32_t)destination.origin.z, 1 };

        vkCmdCopyImage(commandBuffer_->GetNative(),
            vkSrcTexture->GetNative(),
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            vkDstTexture->GetNative(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);
        });
}

void VKCommandEncoder::ResolveQuerySet(
    const QuerySetPtr& querySet,
    std::uint32_t queryFirstIndex,
    std::uint32_t queryCount,
    const BufferPtr& dstBuffer,
    std::uint32_t dstOffset)
{
    /*GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdResolveQuerySet>(querySet, queryFirstIndex, queryCount,
                                                              dstBuffer, dstOffset);*/

    commandBuffer_->AddCmd([querySet, queryFirstIndex, queryCount, dstBuffer, dstOffset](CommandBuffer* cmdBuff) {
        auto commandBuffer_ = (VKCommandBuffer*)cmdBuff;
        commandBuffer_->ResolveQuerySet(querySet, queryFirstIndex, queryCount, dstBuffer, dstOffset);
        });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBuffer(dstBuffer);
}

void VKCommandEncoder::PushDebugGroup(const std::string &groupLabel)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdPushDebugGroup>(groupLabel);

    commandBuffer_->AddCmd([groupLabel](CommandBuffer* cmdBuff) {
        cmdBuff->PushDebugGroup(groupLabel.c_str());
        });
}

void VKCommandEncoder::PopDebugGroup()
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdPopDebugGroup>();

    commandBuffer_->AddCmd([](CommandBuffer* cmdBuff) {
        cmdBuff->PopDebugGroup();
        });
}

void VKCommandEncoder::InsertDebugMarker(const std::string &markerLabel)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdInsertDebugMarker>(markerLabel);

    commandBuffer_->AddCmd([markerLabel](CommandBuffer* cmdBuff) {
        cmdBuff->InsertDebugMarker(markerLabel.c_str());
        });
}

NS_GFX_END