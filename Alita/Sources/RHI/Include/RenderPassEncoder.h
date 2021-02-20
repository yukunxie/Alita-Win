//
// Created by realxie on 2019-10-18.
//

#ifndef ALITA_RENDERPASSENCODER_H
#define ALITA_RENDERPASSENCODER_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "RenderPipeine.h"
#include "Buffer.h"

#include <stdint.h>

NS_RHI_BEGIN

class RenderPassEncoder : public RHIObjectBase
{
public:
    virtual ~RenderPassEncoder() = default;

public:
    virtual void SetGraphicPipeline(const RenderPipeline* pipeline) = 0;
    
    virtual void SetIndexBuffer(const Buffer* buffer, std::uint32_t offset = 0) = 0;
    
    virtual void
    SetVertexBuffer(const Buffer* buffer, std::uint32_t offset = 0, std::uint32_t slot = 0) = 0;
    
    virtual void
    Draw(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex,
         std::uint32_t firstInstance) = 0;
    
    virtual void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex) = 0;
    
    virtual void DrawIndxed(std::uint32_t indexCount, std::uint32_t firstIndex) = 0;
    
    virtual void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                             std::uint32_t firstIndex, std::uint32_t baseVertex,
                             std::uint32_t firstInstance) = 0;
    
    virtual void
    SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;
    
    virtual void
    SetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width, std::uint32_t height) = 0;
    
    virtual void SetStencilReference(std::uint32_t reference) = 0;
    
    virtual void EndPass() = 0;
    
    virtual void SetBindGroup(std::uint32_t index, const BindGroup* bindGroup,
                              const std::vector<std::uint32_t> &dynamicOffsets = {}) = 0;
    
    //    void setBlendColor(GPUColor color);
    //    void executeBundles(sequence<GPURenderBundle> bundles);
};

NS_RHI_END


#endif //ALITA_RENDERPASSENCODER_H
