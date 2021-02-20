//
// Created by realxie on 2019-10-18.
//

#ifndef ALITA_VKRENDERPASSENCODER_H
#define ALITA_VKRENDERPASSENCODER_H


#include "VKDevice.h"

NS_RHI_BEGIN

class VKRenderPass;

class VKRenderPassEncoder : public RenderPassEncoder
{
public:
    VKRenderPassEncoder(VKDevice* device);
    
    virtual ~VKRenderPassEncoder();
    
    void BeginPass(VkCommandBuffer vkCommandBuffer, const RenderPassDescriptor &descriptor);

public:
    virtual void SetGraphicPipeline(const RenderPipeline* pipeline) override;
    
    virtual void SetIndexBuffer(const Buffer* buffer, std::uint32_t offset = 0) override;
    
    virtual void SetVertexBuffer(const Buffer* buffer, std::uint32_t offset = 0,
                                 std::uint32_t slot = 0) override;
    
    virtual void
    Draw(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex,
         std::uint32_t firstInstance) override;
    
    virtual void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex) override;
    
    virtual void DrawIndxed(std::uint32_t indexCount, std::uint32_t firstIndex) override;
    
    virtual void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                             std::uint32_t firstIndex, std::uint32_t baseVertex,
                             std::uint32_t firstInstance) override;
    
    virtual void SetViewport(float x, float y, float width, float height, float minDepth,
                             float maxDepth) override;
    
    virtual void SetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width,
                                std::uint32_t height) override;
    
    virtual void SetStencilReference(std::uint32_t reference) override;
    
    virtual void SetBindGroup(std::uint32_t index, const BindGroup* bindGroup,
                              const std::vector<std::uint32_t> &dynamicOffsets = {}) override;
    
    virtual void EndPass() override;

private:
    VKDevice* device_ = nullptr;
    VkDevice vkDevice_ = nullptr;
    VkCommandBuffer vkCommandBuffer_ = 0L;
    //    VkRenderPass    vkRenderPass_       = 0L;
    VkFramebuffer vkFramebuffer_ = 0L;
    
    const RenderPipeline* graphicPipeline_ = nullptr;
    
    class VKRenderPass* renderPass_ = nullptr;
};

NS_RHI_END


#endif //ALITA_VKRENDERPASSENCODER_H
