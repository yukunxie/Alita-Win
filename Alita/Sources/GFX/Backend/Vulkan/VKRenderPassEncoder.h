//
// Created by realxie on 2019-10-18.
//

#ifndef RHI_VKRENDERPASSENCODER_H
#define RHI_VKRENDERPASSENCODER_H


#include "VKDevice.h"
#include "VKFramebuffer.h"

NS_GFX_BEGIN

class VKRenderPass;

class VKRenderPipeline;

class VKCommandBuffer;

class VKBuffer;

class VKRenderPassEncoder final : public RenderPassEncoder
{
protected:
    VKRenderPassEncoder(DevicePtr device);
    
public:
    virtual ~VKRenderPassEncoder();

public:
    
    bool Init(CommandBufferPtr commandBuffer, const RenderPassDescriptor &descriptor);
    
    virtual void SetPipeline(RenderPipelinePtr pipeline) override;
    
    virtual void SetIndexBuffer(BufferPtr buffer, std::uint32_t offset = 0) override;
    
    virtual void SetVertexBuffer(BufferPtr buffer, std::uint32_t offset = 0,
                                 std::uint32_t slot = 0) override;
    
    virtual void
    Draw(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex,
         std::uint32_t firstInstance) override;
    
    virtual void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex) override;
    
    virtual void DrawIndxed(std::uint32_t indexCount, std::uint32_t firstIndex) override;
    
    virtual void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                             std::uint32_t firstIndex, int32_t baseVertex,
                             std::uint32_t firstInstance) override;
    
    virtual void DrawIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset) override;
    
    virtual void DrawIndexedIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset) override;
    
    virtual void SetViewport(float x, float y, float width, float height, float minDepth,
                             float maxDepth) override;
    
    virtual void SetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width,
                                std::uint32_t height) override;
    
    virtual void SetStencilReference(std::uint32_t reference) override;
    
    virtual void
    SetBindGroup(std::uint32_t index, BindGroupPtr bindGroup, std::uint32_t dynamicOffsetCount,
                 const std::uint32_t* dynamicOffsets) override;
    
    virtual void PushDebugGroup(const std::string &groupLabel) override;
    
    virtual void PopDebugGroup() override;
    
    virtual void InsertDebugMarker(const std::string &markerLabel) override;
    
    virtual void SetBlendColor(const Color &color) override;
    
    virtual void BeginOcclusionQuery(std::uint32_t queryIndex) override;
    
    virtual void EndOcclusionQuery(std::uint32_t queryIndex) override;

    virtual void SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override;
    
    virtual void EndPass() override;
    
    virtual void Dispose() override;
    
private:
    CommandBufferPtr commandBuffer_ = nullptr;
    FramebufferPtr vkFramebuffer_ = VK_NULL_HANDLE;
    RenderPassPtr renderPass_ = nullptr;
    std::vector<TextureViewPtr> attachments_;
    std::vector<TextureViewPtr> resolveTargets_;
    TextureViewPtr depthStencilAttachemnt_ = nullptr;
    
    bool hasSwapchainImage_ = false;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKRENDERPASSENCODER_H
