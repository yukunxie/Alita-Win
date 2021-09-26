//
// Created by realxie on 2019-10-11.
//

#ifndef RHI_COMMANDBUFFER_H
#define RHI_COMMANDBUFFER_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "RenderPipeline.h"
#include "ComputePipeline.h"
#include "RenderPass.h"
#include "Framebuffer.h"

// class RenderPipeline;
//
// class ComputePipeline;

NS_RHI_BEGIN

class CommandBuffer : public RHIObjectBase
{
public:
    CommandBuffer(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::CommandBuffer)
    {}
    
    virtual bool HasDrawCmdInPresentableImage()
    { return true; }
    
    virtual void
    BeginRenderPass(RenderPass* pass, Framebuffer* framebuffer, QuerySet* occlusionQuerySet,
                    std::uint32_t clearValueCount, Color* clearValues, float clearDepth,
                    uint32_t clearStencil) = 0;

    
    // virtual void
    // BindDescriptorSets(RenderPipeline* pipeline, std::uint32_t index, BindGroup* bindGroup,
    //                    uint32_t dynamicOffsetCount = 0,
    //                    const uint32_t* pDynamicOffsets = nullptr) = 0;
    //
    // virtual void
    // BindDescriptorSets(ComputePipeline* pipeline, std::uint32_t index, BindGroup* bindGroup,
    //                    uint32_t dynamicOffsetCount = 0,
    //                    const uint32_t* pDynamicOffsets = nullptr) = 0;
    
    virtual void SetBindGroupToGraphicPipeline(std::uint32_t index, BindGroup* bindGroup,
                              uint32_t dynamicOffsetCount = 0,
                              const uint32_t* pDynamicOffsets = nullptr) = 0;
    
    virtual void SetBindGroupToComputePipeline(std::uint32_t index, BindGroup* bindGroup,
                                               uint32_t dynamicOffsetCount = 0,
                                               const uint32_t* pDynamicOffsets = nullptr) = 0;
    
    
    virtual void BindGraphicsPipeline(RenderPipeline* graphicPipeline) = 0;
    
    virtual void BindComputePipeline(ComputePipeline* computePipeline) = 0;
    
    // virtual void SetPipeline(RenderPipeline* pipeline) override;
    
    virtual void SetIndexBuffer(Buffer* buffer, std::uint32_t offset = 0) = 0;
    
    virtual void SetVertexBuffer(Buffer* buffer, std::uint32_t offset = 0,
                                 std::uint32_t slot = 0) = 0;
    
    virtual void
    Dispatch(std::uint32_t groupCountX, std::uint32_t groupCountY, std::uint32_t groupCountZ) = 0;
    
    virtual void DispatchIndirect(Buffer* indirectBuffer, BufferSize indirectOffset) = 0;
    
    // virtual void ResetCommandBuffer() = 0;
    //
    // virtual void BeginCommandBuffer() = 0;
    //
    // virtual void EndCommandBuffer() = 0;
    
    // virtual void Dispose() override;
    
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
    
    virtual void SetViewport(float x, float y, float width, float height, float minDepth,
                             float maxDepth) = 0;
    
    virtual void SetScissorRect(uint32_t x, uint32_t y, std::uint32_t width,
                                std::uint32_t height) = 0;
    
    virtual void SetStencilReference(std::uint32_t reference) = 0;
   
    virtual void SetBlendColor(const Color &color) = 0;

    virtual void SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) = 0;
    
    virtual void PushDebugGroup(const char* groupLabel) = 0;
    
    virtual void PopDebugGroup() = 0;
    
    virtual void InsertDebugMarker(const char* markerLabel) = 0;
    
    virtual void BeginOcclusionQuery(std::uint32_t queryIndex) = 0;
    
    virtual void EndOcclusionQuery(std::uint32_t queryIndex) = 0;
    
    virtual void ResolveQuerySet(
        QuerySet* querySet,
        std::uint32_t queryFirstIndex,
        std::uint32_t queryCount,
        Buffer* dstBuffer,
        std::uint32_t dstOffset) = 0;
    
protected:
    virtual ~CommandBuffer()
    {}
};

NS_RHI_END


#endif //RHI_COMMANDBUFFER_H
