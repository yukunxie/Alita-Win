//
// Created by realxie on 2019-10-14.
//

#ifndef RHI_VKCOMMANDENCODER_H
#define RHI_VKCOMMANDENCODER_H

#include "VKDevice.h"

NS_GFX_BEGIN

class VKCommandBuffer;

class VKRenderPassEncoder;

class VKComputePassEncoder;

class VKCommandEncoder final : public CommandEncoder
{
protected:
    VKCommandEncoder(const DevicePtr& device);
    
public:
    ~VKCommandEncoder();

public:
    
    bool Init();
    
    virtual void Reset() override;

    virtual RenderPassEncoderPtr BeginRenderPass(const RenderPassDescriptor &descriptor) override;
    
    virtual ComputePassEncoderPtr BeginComputePass(const ComputePassDescriptor &descriptor) override;
    
    virtual CommandBufferPtr Finish(const CommandBufferDescriptor &descriptor = {}) override;
    
    virtual void CopyBufferToBuffer(
        BufferPtr source,
        BufferSize sourceOffset,
        BufferPtr destination,
        BufferSize destinationOffset,
        BufferSize size) override;
    
    virtual void CopyBufferToTexture(
        BufferCopyView &source,
        TextureCopyView &destination,
        Extent3D &copySize) override;
    
    virtual void CopyTextureToBuffer(
        TextureCopyView &source,
        BufferCopyView &destination,
        Extent3D &copySize) override;
    
    virtual void CopyTextureToTexture(
        TextureCopyView &source,
        TextureCopyView &destination,
        Extent3D &copySize) override;
    
    virtual void ResolveQuerySet(
        const QuerySetPtr& querySet,
        std::uint32_t queryFirstIndex,
        std::uint32_t queryCount,
        const BufferPtr& dstBuffer,
        std::uint32_t dstOffset) override;
    
    virtual void PushDebugGroup(const std::string &groupLabel) override;
    
    virtual void PopDebugGroup() override;
    
    virtual void InsertDebugMarker(const std::string &markerLabel) override;
    
    virtual void Dispose() override;

private:
    CommandBufferPtr commandBuffer_ = nullptr;
    RenderPassEncoderPtr renderPassEncoder_ = nullptr;
    ComputePassEncoderPtr computePassEncoder_ = nullptr;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKCOMMANDENCODER_H
