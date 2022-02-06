//
// Created by realxie on 2019-10-11.
//

#ifndef RHI_COMMANDENCODER_H
#define RHI_COMMANDENCODER_H

#include "Macros.h"
#include "GFXBase.h"
#include "CommandBuffer.h"
#include "RenderPassEncoder.h"
#include "ComputePassEncoder.h"
#include "Descriptors.h"
#include "Buffer.h"

NS_GFX_BEGIN

class CommandEncoder : public GfxBase
{
public:
    CommandEncoder(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::CommandEncoder)
    {}

    virtual ~CommandEncoder() = default;

public:
    virtual RenderPassEncoderPtr BeginRenderPass(const RenderPassDescriptor &descriptor) = 0;
    
    virtual ComputePassEncoderPtr BeginComputePass(const ComputePassDescriptor &descriptor) = 0;
    
    virtual CommandBufferPtr Finish(const CommandBufferDescriptor &descriptor = {}) = 0;
    
    virtual void CopyBufferToBuffer(
        BufferPtr source,
        BufferSize sourceOffset,
        BufferPtr destination,
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
        const QuerySetPtr& querySet,
        std::uint32_t queryFirstIndex,
        std::uint32_t queryCount,
        const BufferPtr& dstBuffer,
        std::uint32_t dstOffset) = 0;
    
    virtual void PushDebugGroup(const std::string &groupLabel) = 0;
    
    virtual void PopDebugGroup() = 0;
    
    virtual void InsertDebugMarker(const std::string &markerLabel) = 0;
};

NS_GFX_END


#endif //RHI_COMMANDENCODER_H
