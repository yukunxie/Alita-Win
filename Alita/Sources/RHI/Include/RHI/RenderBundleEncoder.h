//
// Created by realxie on 2020-01-22.
//

#ifndef RHI_RENDERBUNDLEENCODER_H
#define RHI_RENDERBUNDLEENCODER_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "RenderPipeline.h"
#include "Buffer.h"
#include "RenderPassEncoder.h"
#include "RenderBundle.h"

NS_RHI_BEGIN

class RenderBundleEncoder : public RHIObjectBase
{
public:
    RenderBundleEncoder(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::RenderBundleEncoder)
    {
    }

protected:
    ~RenderBundleEncoder() = default;

public:
    virtual void SetPipeline(RenderPipeline* pipeline) = 0;
    
    virtual void SetIndexBuffer(Buffer* buffer, std::uint32_t offset = 0) = 0;
    
    virtual void
    SetVertexBuffer(Buffer* buffer, std::uint32_t offset = 0, std::uint32_t slot = 0) = 0;
    
    virtual void
    Draw(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex,
         std::uint32_t firstInstance) = 0;
    
    virtual void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex) = 0;
    
    virtual void DrawIndxed(std::uint32_t indexCount, std::uint32_t firstIndex) = 0;
    
    virtual void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                             std::uint32_t firstIndex, int32_t baseVertex,
                             std::uint32_t firstInstance) = 0;
    
    virtual void DrawIndirect(Buffer* indirectBuffer, BufferSize indirectOffset) = 0;
    
    virtual void DrawIndexedIndirect(Buffer* indirectBuffer, BufferSize indirectOffset) = 0;
    
    virtual void
    SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;
    
    virtual void
    SetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width, std::uint32_t height) = 0;
    
    virtual void SetBindGroup(std::uint32_t index, BindGroup* bindGroup, std::uint32_t count,
                              const uint32_t* dynamicOffsets) = 0;
    
    // virtual void SetBindGroup(std::uint32_t index, BindGroup* bindGroup,
    //                           const std::uint32_t* dynamicOffsetsData,
    //                           std::uint64_t dynamicOffsetsDataStart,
    //                           std::uint64_t dynamicOffsetsDataLength) = 0;
    
    virtual void PushDebugGroup(const std::string &groupLabel) = 0;
    
    virtual void PopDebugGroup() = 0;
    
    virtual void InsertDebugMarker(const std::string &markerLabel) = 0;
    
    virtual RenderBundle* Finish(const RenderBundleDescriptor &descriptor) = 0;
    
    virtual void Reset() = 0;
};

NS_RHI_END

#endif //RHI_RENDERBUNDLEENCODER_H
