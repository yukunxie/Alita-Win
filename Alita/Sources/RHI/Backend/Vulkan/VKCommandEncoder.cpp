//
// Created by realxie on 2019-10-14.
//

#include "VKCommandEncoder.h"
#include "VKCommandBuffer.h"
#include "VKRenderPassEncoder.h"
#include "VKBuffer.h"
#include "VKTexture.h"


NS_RHI_BEGIN

VKCommandEncoder::VKCommandEncoder(VKDevice* device)
{
    device_ = device;
    
    commandBuffer_ = new VKCommandBuffer(device_);
    RHI_SAFE_RETAIN(commandBuffer_);
}

VKCommandEncoder::~VKCommandEncoder()
{
    RHI_SAFE_RELEASE(commandBuffer_);
    RHI_SAFE_RELEASE(renderPassEncoder_);
}

RenderPassEncoder* VKCommandEncoder::BeginRenderPass(const RenderPassDescriptor &descriptor)
{
    if (renderPassEncoder_ == nullptr)
    {
        renderPassEncoder_ = new VKRenderPassEncoder(device_);
        RHI_SAFE_RETAIN(renderPassEncoder_);
    }
    commandBuffer_->ResetCommandBuffer();
    renderPassEncoder_->BeginPass(commandBuffer_->GetNative(), descriptor);
    return renderPassEncoder_;
}

CommandBuffer* VKCommandEncoder::Finish(const CommandBufferDescriptor &descriptor)
{
    return commandBuffer_;
}

void VKCommandEncoder::CopyBufferToBuffer(
    const Buffer* source,
    BufferSize sourceOffset,
    Buffer* destination,
    BufferSize destinationOffset,
    BufferSize size)
{
}

void VKCommandEncoder::CopyBufferToTexture(
    const BufferCopyView &source,
    TextureCopyView &destination,
    const Extent3D &copySize)
{
    auto buffer = RHI_CAST(const VKBuffer*, source.buffer);
    auto texture = RHI_CAST(const VKTexture*, destination.texture);
    auto dstImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    
    VkBufferImageCopy bufferImageCopy;
    {
        bufferImageCopy.bufferOffset = source.offset;
        // bufferRowLength measured by pixel.
        bufferImageCopy.bufferRowLength =
            source.rowPitch / GetTextureFormatPixelSize(texture->GetFormat());
        bufferImageCopy.bufferImageHeight = source.imageHeight;
        bufferImageCopy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, destination.mipLevel, 0,
                                            destination.arrayLayer};
        bufferImageCopy.imageOffset = {destination.origin.x, destination.origin.y,
                                       destination.origin.z};
        bufferImageCopy.imageExtent = {copySize.width, copySize.height, copySize.depth};
    }
    vkCmdCopyBufferToImage(commandBuffer_->GetNative(), buffer->GetNative(), texture->GetNative(),
                           dstImageLayout, 1, &bufferImageCopy);
}

NS_RHI_END