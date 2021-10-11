//
// Created by realxie on 2020-01-22.
//

#ifndef RHI_VKRENDERBUNDLEENCODER_H
#define RHI_VKRENDERBUNDLEENCODER_H

#include "VKDevice.h"
#include "VKFramebuffer.h"

NS_GFX_BEGIN

class VKRenderBundleEncoder final : public RenderBundleEncoder
{
protected:
    VKRenderBundleEncoder() = delete;
    
    VKRenderBundleEncoder(VKDevice* device);
    
    virtual ~VKRenderBundleEncoder();

public:
    
    bool Init(const RenderBundleEncoderDescriptor &descriptor);
    
    virtual void SetPipeline(RenderPipeline* pipeline) override;
    
    virtual void SetIndexBuffer(Buffer* buffer, std::uint32_t offset = 0) override;
    
    virtual void
    SetVertexBuffer(Buffer* buffer, std::uint32_t offset = 0, std::uint32_t slot = 0) override;
    
    virtual void
    Draw(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex,
         std::uint32_t firstInstance) override;
    
    virtual void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex) override;
    
    virtual void DrawIndxed(std::uint32_t indexCount, std::uint32_t firstIndex) override;
    
    virtual void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                             std::uint32_t firstIndex, int32_t baseVertex,
                             std::uint32_t firstInstance) override;
    
    virtual void DrawIndirect(Buffer* indirectBuffer, BufferSize indirectOffset) override;
    
    virtual void DrawIndexedIndirect(Buffer* indirectBuffer, BufferSize indirectOffset) override;
    
    virtual void
    SetViewport(float x, float y, float width, float height, float minDepth,
                float maxDepth) override;
    
    virtual void
    SetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width,
                   std::uint32_t height) override;
    
    virtual void SetBindGroup(std::uint32_t index, BindGroup* bindGroup, std::uint32_t count,
                              const uint32_t* dynamicOffsets) override;
    
    virtual void PushDebugGroup(const std::string &groupLabel) override;
    
    virtual void PopDebugGroup() override;
    
    virtual void InsertDebugMarker(const std::string &markerLabel) override;
    
    virtual RenderBundle* Finish(const RenderBundleDescriptor &descriptor) override;
    
    virtual void Reset() override;
    
    virtual void Dispose() override;

private:
    
    void CheckRenderBundle();

private:
    RenderBundle* renderBundle_ = nullptr;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKRENDERBUNDLEENCODER_H
