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

VKCommandEncoder::VKCommandEncoder(VKDevice* device)
    : CommandEncoder(device)
{
    GFX_PTR_ASSIGN(commandBuffer_, VKDEVICE()->CreateCommandBuffer());
}

void VKCommandEncoder::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    GFX_SAFE_RELEASE(commandBuffer_);
    GFX_SAFE_RELEASE(renderPassEncoder_);
    GFX_SAFE_RELEASE(computePassEncoder_);
    
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
    GFX_SAFE_RELEASE(commandBuffer_);
    GFX_SAFE_RELEASE(renderPassEncoder_);
    GFX_SAFE_RELEASE(computePassEncoder_);
    
    GFX_PTR_ASSIGN(commandBuffer_, VKDEVICE()->CreateCommandBuffer());
}

RenderPassEncoder* VKCommandEncoder::BeginRenderPass(const RenderPassDescriptor &descriptor)
{
    if (renderPassEncoder_)
    {
        renderPassEncoder_->EndPass();
    }
    GFX_SAFE_RELEASE(renderPassEncoder_);
    
    auto newRenderPassEncoder = VKDEVICE()->CreateObject<VKRenderPassEncoder>(commandBuffer_, descriptor);
    GFX_PTR_ASSIGN(renderPassEncoder_, newRenderPassEncoder);
    
    return renderPassEncoder_;
}

ComputePassEncoder* VKCommandEncoder::BeginComputePass(const ComputePassDescriptor &descriptor)
{
    if (computePassEncoder_)
    {
        computePassEncoder_->EndPass();
    }
    
    auto newComputePassEncoder = VKDEVICE()->CreateObject<VKComputePassEncoder>(commandBuffer_, descriptor);
    GFX_PTR_ASSIGN(computePassEncoder_, newComputePassEncoder);
    
    return computePassEncoder_;
}

CommandBuffer* VKCommandEncoder::Finish(const CommandBufferDescriptor &descriptor)
{
    return commandBuffer_;
}

void VKCommandEncoder::CopyBufferToBuffer(
    Buffer* source,
    BufferSize sourceOffset,
    Buffer* destination,
    BufferSize destinationOffset,
    BufferSize size)
{
    commandBuffer_->RecordCommand<DeferredCmdCopyBufferToBuffer>(source, sourceOffset, destination,
                                                                 destinationOffset, size);
    commandBuffer_->AddBindingObject(source);
    commandBuffer_->AddBindingObject(destination);
}

void VKCommandEncoder::CopyBufferToTexture(
    BufferCopyView &source,
    TextureCopyView &destination,
    Extent3D &copySize)
{
    commandBuffer_->RecordCommand<DeferredCmdCopyBufferToTexture>(source, destination, copySize);
    commandBuffer_->AddBindingObject(source.buffer);
    commandBuffer_->AddBindingObject(destination.texture);
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
    commandBuffer_->RecordCommand<DeferredCmdCopyTextureToBuffer>(source, destination, copySize);
    commandBuffer_->AddBindingObject(source.texture);
    commandBuffer_->AddBindingObject(destination.buffer);
}

void VKCommandEncoder::CopyTextureToTexture(
    TextureCopyView &source,
    TextureCopyView &destination,
    Extent3D &copySize)
{
    commandBuffer_->RecordCommand<DeferredCmdCopyTextureToTexture>(source, destination, copySize);
    commandBuffer_->AddBindingObject(source.texture);
    commandBuffer_->AddBindingObject(destination.texture);
}

void VKCommandEncoder::ResolveQuerySet(
    QuerySet* querySet,
    std::uint32_t queryFirstIndex,
    std::uint32_t queryCount,
    Buffer* dstBuffer,
    std::uint32_t dstOffset)
{
    commandBuffer_->RecordCommand<DeferredCmdResolveQuerySet>(querySet, queryFirstIndex, queryCount,
                                                              dstBuffer, dstOffset);
    commandBuffer_->AddBindingObject(querySet);
    commandBuffer_->AddBindingObject(dstBuffer);
}

void VKCommandEncoder::PushDebugGroup(const std::string &groupLabel)
{
    commandBuffer_->RecordCommand<DeferredCmdPushDebugGroup>(groupLabel);
}

void VKCommandEncoder::PopDebugGroup()
{
    commandBuffer_->RecordCommand<DeferredCmdPopDebugGroup>();
}

void VKCommandEncoder::InsertDebugMarker(const std::string &markerLabel)
{
    commandBuffer_->RecordCommand<DeferredCmdInsertDebugMarker>(markerLabel);
}

NS_GFX_END