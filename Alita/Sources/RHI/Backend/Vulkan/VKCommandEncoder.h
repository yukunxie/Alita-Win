//
// Created by realxie on 2019-10-14.
//

#ifndef RHI_VKCOMMANDENCODER_H
#define RHI_VKCOMMANDENCODER_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKCommandBuffer;

class VKRenderPassEncoder;

class VKComputePassEncoder;

class VKCommandEncoder final : public CommandEncoder
{
protected:
    VKCommandEncoder(VKDevice* device);
    
    ~VKCommandEncoder();

public:
    
    bool Init();
    
    virtual void Reset() override;
    
    virtual RenderPassEncoder* BeginRenderPass(const RenderPassDescriptor &descriptor) override;
    
    virtual ComputePassEncoder* BeginComputePass(const ComputePassDescriptor &descriptor) override;
    
    virtual CommandBuffer* Finish(const CommandBufferDescriptor &descriptor = {}) override;
    
    virtual void CopyBufferToBuffer(
        Buffer* source,
        BufferSize sourceOffset,
        Buffer* destination,
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
        QuerySet* querySet,
        std::uint32_t queryFirstIndex,
        std::uint32_t queryCount,
        Buffer* dstBuffer,
        std::uint32_t dstOffset) override;
    
    virtual void PushDebugGroup(const std::string &groupLabel) override;
    
    virtual void PopDebugGroup() override;
    
    virtual void InsertDebugMarker(const std::string &markerLabel) override;
    
    virtual void Dispose() override;

private:
    VKCommandBuffer* commandBuffer_ = nullptr;
    VKRenderPassEncoder* renderPassEncoder_ = nullptr;
    VKComputePassEncoder* computePassEncoder_ = nullptr;
    
    friend class VKDevice;
};

NS_RHI_END


#endif //RHI_VKCOMMANDENCODER_H
