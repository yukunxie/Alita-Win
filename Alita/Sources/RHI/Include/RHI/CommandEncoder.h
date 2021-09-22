//
// Created by realxie on 2019-10-11.
//

#ifndef RHI_COMMANDENCODER_H
#define RHI_COMMANDENCODER_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "CommandBuffer.h"
#include "RenderPassEncoder.h"
#include "ComputePassEncoder.h"
#include "Descriptors.h"
#include "Buffer.h"

NS_RHI_BEGIN

class CommandEncoder : public RHIObjectBase
{
public:
    CommandEncoder(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::CommandEncoder)
    {}

protected:
    virtual ~CommandEncoder() = default;

public:
    virtual RenderPassEncoder* BeginRenderPass(const RenderPassDescriptor &descriptor) = 0;
    
    virtual ComputePassEncoder* BeginComputePass(const ComputePassDescriptor &descriptor) = 0;
    
    virtual CommandBuffer* Finish(const CommandBufferDescriptor &descriptor = {}) = 0;
    
    virtual void CopyBufferToBuffer(
        Buffer* source,
        BufferSize sourceOffset,
        Buffer* destination,
        BufferSize destinationOffset,
        BufferSize size) = 0;
    
    virtual void CopyBufferToTexture(
        BufferCopyView &source,
        TextureCopyView &destination,
        Extent3D &copySize) = 0;
    
    virtual void CopyTextureToBuffer(
        TextureCopyView &source,
        BufferCopyView &destination,
        Extent3D &copySize) = 0;
    
    virtual void CopyTextureToTexture(
        TextureCopyView &source,
        TextureCopyView &destination,
        Extent3D &copySize) = 0;
    
    virtual void ResolveQuerySet(
        QuerySet* querySet,
        std::uint32_t queryFirstIndex,
        std::uint32_t queryCount,
        Buffer* dstBuffer,
        std::uint32_t dstOffset) = 0;
    
    virtual void PushDebugGroup(const std::string &groupLabel) = 0;
    
    virtual void PopDebugGroup() = 0;
    
    virtual void InsertDebugMarker(const std::string &markerLabel) = 0;
};

NS_RHI_END


#endif //RHI_COMMANDENCODER_H
