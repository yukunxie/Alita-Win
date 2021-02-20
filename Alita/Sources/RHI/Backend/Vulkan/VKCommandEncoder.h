//
// Created by realxie on 2019-10-14.
//

#ifndef ALITA_VKCOMMANDENCODER_H
#define ALITA_VKCOMMANDENCODER_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKCommandBuffer;

class VKRenderPassEncoder;

class VKCommandEncoder : public CommandEncoder
{
public:
    VKCommandEncoder(VKDevice* device);
    
    ~VKCommandEncoder();

public:
    virtual RenderPassEncoder* BeginRenderPass(const RenderPassDescriptor &descriptor) override;
    
    virtual CommandBuffer* Finish(const CommandBufferDescriptor &descriptor = {}) override;
    
    virtual void CopyBufferToBuffer(
        const Buffer* source,
        BufferSize sourceOffset,
        Buffer* destination,
        BufferSize destinationOffset,
        BufferSize size) override;
    
    virtual void CopyBufferToTexture(
        const BufferCopyView &source,
        TextureCopyView &destination,
        const Extent3D &copySize) override;

private:
    VKDevice* device_ = nullptr;
    VKCommandBuffer* commandBuffer_ = nullptr;
    VKRenderPassEncoder* renderPassEncoder_ = nullptr;
};

NS_RHI_END


#endif //ALITA_VKCOMMANDENCODER_H
