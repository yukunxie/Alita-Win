//
// Created by realxie on 2019-10-18.
//

#ifndef RHI_RENDERPASSENCODER_H
#define RHI_RENDERPASSENCODER_H

#include "Macros.h"
#include "GFXBase.h"
#include "RenderPipeline.h"
#include "Buffer.h"

#include <stdint.h>

NS_GFX_BEGIN

class RenderPassEncoder : public GfxBase
{
public:
    RenderPassEncoder(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::RenderPassEncoder)
    {}

    virtual ~RenderPassEncoder() = default;

public:
    virtual void SetPipeline(RenderPipelinePtr pipeline) = 0;
    
    virtual void SetIndexBuffer(BufferPtr buffer, std::uint32_t offset = 0) = 0;
    
    virtual void
    SetVertexBuffer(BufferPtr buffer, std::uint32_t offset = 0, std::uint32_t slot = 0) = 0;
    
    virtual void
    Draw(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex,
         std::uint32_t firstInstance) = 0;
    
    virtual void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex) = 0;
    
    virtual void DrawIndxed(std::uint32_t indexCount, std::uint32_t firstIndex) = 0;
    
    virtual void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                             std::uint32_t firstIndex, int32_t baseVertex,
                             std::uint32_t firstInstance) = 0;
    
    virtual void DrawIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset) = 0;
    
    virtual void DrawIndexedIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset) = 0;
    
    virtual void
    SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;
    
    virtual void
    SetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width, std::uint32_t height) = 0;
    
    virtual void SetStencilReference(std::uint32_t reference) = 0;
    
    virtual void EndPass() = 0;
    
    virtual void
    SetBindGroup(std::uint32_t index, BindGroupPtr bindGroup, uint32_t dynamicOffsetCount,
                 const std::uint32_t* dynamicOffsets) = 0;
    
    // virtual void SetBindGroup(std::uint32_t index, BindGroup* bindGroup,
    //                           const std::uint32_t* dynamicOffsetsData,
    //                           uint32_t dynamicOffsetsDataStart,
    //                           uint32_t dynamicOffsetsDataLength) = 0;
    
    virtual void PushDebugGroup(const std::string &groupLabel) = 0;
    
    virtual void PopDebugGroup() = 0;
    
    virtual void InsertDebugMarker(const std::string &markerLabel) = 0;
    
    virtual void SetBlendColor(const Color &color) = 0;
    
    virtual void BeginOcclusionQuery(std::uint32_t queryIndex) = 0;
    
    virtual void EndOcclusionQuery(std::uint32_t queryIndex) = 0;

    virtual void SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) = 0;
};

NS_GFX_END


#endif //RHI_RENDERPASSENCODER_H
