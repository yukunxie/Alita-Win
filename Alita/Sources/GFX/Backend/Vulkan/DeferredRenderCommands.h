//
// Created by realxie on 2020-02-28.
//

#ifndef PROJ_ANDROID_DEFERREDRENDERCOMMANDS_H
#define PROJ_ANDROID_DEFERREDRENDERCOMMANDS_H

#include "GFX/GFX.h"

#include <array>
#include <vulkan/vulkan_core.h>


class VKCommandBuffer;

class VKBindGroup;

NS_GFX_BEGIN

struct DeferredCmdBase
{
public:
    RenderCommand commandType = RenderCommand::INVALID;
    
    virtual void Execute(VKCommandBuffer* commandBuffer)
    {
        GFX_ASSERT(false);
    }
    
    ~DeferredCmdBase()
    {
    }

protected:
    explicit DeferredCmdBase(RenderCommand commandType)
        : commandType(commandType)
    {
    }
};

struct DeferredCmdBeginCommandBuffer final : public DeferredCmdBase
{
    DeferredCmdBeginCommandBuffer()
        : DeferredCmdBase(RenderCommand::BeginCommandBuffer)
    {
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
};

struct DeferredCmdBeginRenderPass final : public DeferredCmdBase
{
    DeferredCmdBeginRenderPass(RenderPassPtr renderPass, FramebufferPtr framebuffer,
                               QuerySetPtr occlusionQuerySet, std::uint32_t clearValueCount,
                               Color* clearValues, float clearDepth, uint32_t clearStencil)
        : DeferredCmdBase(RenderCommand::BeginRenderPass)
    {
        renderPass_ = renderPass;
        framebuffer_ = framebuffer;
        occlusionQuerySet_ = occlusionQuerySet;
        clearValueCount_ = clearValueCount;
        GFX_ASSERT(clearValueCount <= clearColors_.size());
        memcpy(clearColors_.data(), clearValues, clearValueCount * sizeof(clearColors_[0]));
        clearDepth_ = clearDepth;
        clearStencil_ = clearStencil;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
protected:
    RenderPassPtr renderPass_ = nullptr;
    FramebufferPtr framebuffer_ = nullptr;
    QuerySetPtr occlusionQuerySet_ = nullptr;
    std::uint32_t clearValueCount_ = 0;
    std::array<Color, kMaxColorAttachments> clearColors_;
    float clearDepth_ = 1.0f;
    std::uint32_t clearStencil_ = 0;
};

struct DeferredCmdClearAttachment final : public DeferredCmdBase
{
    explicit DeferredCmdClearAttachment(VkClearAttachment clearAttachment)
        : DeferredCmdBase(RenderCommand::ClearAttachment), clearAttachment_(clearAttachment)
    {
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    VkClearAttachment clearAttachment_;
};

struct DeferredCmdSetGraphicPipeline final : public DeferredCmdBase
{
    DeferredCmdSetGraphicPipeline(RenderPipelinePtr graphicPipeline)
        : DeferredCmdBase(RenderCommand::SetGraphicPipeline)
    {
        graphicPipeline_ = graphicPipeline;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    RenderPipelinePtr graphicPipeline_ = nullptr;
};

struct DeferredCmdSetComputePipeline final : public DeferredCmdBase
{
    DeferredCmdSetComputePipeline(ComputePipelinePtr computePipeline)
        : DeferredCmdBase(RenderCommand::SetComputePipeline)
    {
        computePipeline_ = computePipeline;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;

private:
    ComputePipelinePtr computePipeline_ = nullptr;
};

struct DeferredCmdDispatch final : public DeferredCmdBase
{
    DeferredCmdDispatch(std::uint32_t x, std::uint32_t y, std::uint32_t z)
        : DeferredCmdBase(RenderCommand::Dispatch), x_(x), y_(y), z_(z)
    {
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;

private:
    std::uint32_t x_;
    std::uint32_t y_;
    std::uint32_t z_;
};

struct DeferredCmdDispatchIndirect final : public DeferredCmdBase
{
    DeferredCmdDispatchIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset)
        : DeferredCmdBase(RenderCommand::DispatchIndirect)
    {
        indirectBuffer_ = indirectBuffer;
        indirectOffset_ = indirectOffset;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    BufferPtr indirectBuffer_ = nullptr;
    BufferSize indirectOffset_ = 0;
};

struct DeferredCmdDraw final : public DeferredCmdBase
{
    DeferredCmdDraw(std::uint32_t vertexCount, std::uint32_t instanceCount,
                    std::uint32_t firstVertex, std::uint32_t firstInstance)
        : DeferredCmdBase(RenderCommand::Draw)
    {
        vertexCount_ = vertexCount;
        instanceCount_ = instanceCount;
        firstVertex_ = firstVertex;
        firstInstance_ = firstInstance;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    std::uint32_t vertexCount_;
    std::uint32_t instanceCount_;
    std::uint32_t firstVertex_;
    std::uint32_t firstInstance_;
};

struct DeferredCmdDrawIndexed final : public DeferredCmdBase
{
    DeferredCmdDrawIndexed(std::uint32_t indexCount,
                           std::uint32_t instanceCount,
                           std::uint32_t firstIndex,
                           std::int32_t baseVertex,
                           std::uint32_t firstInstance)
        : DeferredCmdBase(RenderCommand::DrawIndexed)
    {
        indexCount_ = indexCount;
        instanceCount_ = instanceCount;
        firstIndex_ = firstIndex;
        baseVertex_ = baseVertex;
        firstInstance_ = firstInstance;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    std::uint32_t indexCount_;
    std::uint32_t instanceCount_;
    std::uint32_t firstIndex_;
    std::int32_t baseVertex_;
    std::uint32_t firstInstance_;
};

struct DeferredCmdEndPass final : public DeferredCmdBase
{
    DeferredCmdEndPass()
        : DeferredCmdBase(RenderCommand::EndPass)
    {
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
};

struct DeferredCmdSetVertexBuffer final : public DeferredCmdBase
{
    DeferredCmdSetVertexBuffer(BufferPtr buffer, std::uint32_t offset, std::uint32_t slot)
        : DeferredCmdBase(RenderCommand::SetVertexBuffer)
    {
        buffer_ = buffer;
        offset_ = offset;
        slot_ = slot;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    BufferPtr buffer_ = nullptr;
    std::uint32_t offset_ = 0;
    std::uint32_t slot_ = 0;
};

struct DeferredCmdSetIndexBuffer final : public DeferredCmdBase
{
    DeferredCmdSetIndexBuffer(BufferPtr buffer, std::uint32_t offset)
        : DeferredCmdBase(RenderCommand::SetIndexBuffer)
    {
        buffer_ = buffer;
        offset_ = offset;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    BufferPtr buffer_ = nullptr;
    std::uint32_t offset_ = 0;
};

struct DeferredCmdDrawIndirect final : public DeferredCmdBase
{
    DeferredCmdDrawIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset)
        : DeferredCmdBase(RenderCommand::DrawIndirect)
    {
        indirectBuffer_ = indirectBuffer;
        indirectOffset_ = indirectOffset;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    BufferPtr indirectBuffer_ = nullptr;
    BufferSize indirectOffset_ = 0;
};

struct DeferredCmdDrawIndexedIndirect final : public DeferredCmdBase
{
    DeferredCmdDrawIndexedIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset)
        : DeferredCmdBase(RenderCommand::DrawIndexedIndirect)
    {
        indirectBuffer_ = indirectBuffer;
        indirectOffset_ = indirectOffset;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    BufferPtr indirectBuffer_ = nullptr;
    BufferSize indirectOffset_ = 0;
};

struct DeferredCmdSetViewport final : public DeferredCmdBase
{
    DeferredCmdSetViewport(float x, float y, float width, float height, float minDepth,
                           float maxDepth)
        : DeferredCmdBase(RenderCommand::SetViewport)
    {
        x_ = x;
        y_ = y;
        width_ = width;
        height_ = height;
        minDepth_ = minDepth;
        maxDepth_ = maxDepth;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    float x_;
    float y_;
    float width_;
    float height_;
    float minDepth_;
    float maxDepth_;
};

struct DeferredCmdSetScissorRect final : public DeferredCmdBase
{
    DeferredCmdSetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width,
                              std::uint32_t height)
        : DeferredCmdBase(RenderCommand::SetScissorRect)
    {
        x_ = x;
        y_ = y;
        width_ = width;
        height_ = height;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    std::int32_t x_;
    std::int32_t y_;
    std::uint32_t width_;
    std::uint32_t height_;
};

struct DeferredCmdSetStencilReference final : public DeferredCmdBase
{
    DeferredCmdSetStencilReference(std::uint32_t reference)
        : DeferredCmdBase(RenderCommand::SetStencilReference)
    {
        reference_ = reference;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;

private:
    std::uint32_t reference_ = 0;
};

struct DeferredCmdSetBindGroup final : public DeferredCmdBase
{
    DeferredCmdSetBindGroup(PipelineType pipelineType, std::uint32_t index,
                            const BindGroupPtr& bindGroup,
                            uint32_t dynamicOffsetCount,
                            const uint32_t* pDynamicOffsets)
        : DeferredCmdBase(RenderCommand::SetBindGroup)
    {
        pipelineType_ = pipelineType;
        index_ = index;
        bindGroup_ = bindGroup;
        dynamicOffsetCount_ = dynamicOffsetCount;
        std::uint32_t* pData = nullptr;
        
        GFX_ASSERT(dynamicOffsetCount_ <= kMaxBindingsPerGroup);
        
        if (dynamicOffsetCount_ <= kMaxBindingsPerGroup)
        {
            memcpy(dynamicOffsets_.data(), pDynamicOffsets,
                   dynamicOffsetCount * sizeof(pDynamicOffsets[0]));
        }
        else
        {
            LOGE("Too much dynamicOffsets_");
        }
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;

private:
    PipelineType pipelineType_ = PipelineType::Graphic;
    std::uint32_t index_ = 0;
    BindGroupPtr bindGroup_ = nullptr;
    uint32_t dynamicOffsetCount_ = 0;
    std::array<std::uint32_t, kMaxBindingsPerGroup> dynamicOffsets_;
};

struct DeferredCmdCopyBufferToTexture final : public DeferredCmdBase
{
    DeferredCmdCopyBufferToTexture(BufferCopyView &source,
                                   TextureCopyView &destination,
                                   Extent3D &copySize)
        : DeferredCmdBase(RenderCommand::CopyBufferToTexture)
    {
        source_ = source;
        destination_ = destination;
        copySize_ = copySize;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;

private:
    BufferCopyView source_;
    TextureCopyView destination_;
    Extent3D copySize_;
};

struct DeferredCmdCopyTextureToBuffer final : public DeferredCmdBase
{
    DeferredCmdCopyTextureToBuffer(TextureCopyView &source,
                                   BufferCopyView &destination,
                                   Extent3D &copySize)
        : DeferredCmdBase(RenderCommand::CopyTextureToBuffer)
    {
        source_ = source;
        destination_ = destination;
        copySize_ = copySize;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;

private:
    TextureCopyView source_;
    BufferCopyView destination_;
    Extent3D copySize_;
};

struct DeferredCmdCopyTextureToTexture final : public DeferredCmdBase
{
    DeferredCmdCopyTextureToTexture(TextureCopyView &source,
                                    TextureCopyView &destination,
                                    Extent3D &copySize)
        : DeferredCmdBase(RenderCommand::CopyTextureToTexture)
    {
        source_ = source;
        destination_ = destination;
        copySize_ = copySize;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;

private:
    TextureCopyView source_;
    TextureCopyView destination_;
    Extent3D copySize_;
};

struct DeferredCmdCopyBufferToBuffer final : public DeferredCmdBase
{
    DeferredCmdCopyBufferToBuffer(BufferPtr source,
                                  BufferSize sourceOffset,
                                  BufferPtr destination,
                                  BufferSize destinationOffset,
                                  BufferSize size)
        : DeferredCmdBase(RenderCommand::CopyBufferToBuffer)
    {
        source_ = source;
        sourceOffset_ = sourceOffset;
        destination_ = destination;
        destinationOffset_ = destinationOffset;
        size_ = size;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;

private:
    BufferPtr source_ = nullptr;
    BufferSize sourceOffset_;
    BufferPtr destination_ = nullptr;
    BufferSize destinationOffset_;
    BufferSize size_;
};

struct DeferredCmdSetBlendColor final : public DeferredCmdBase
{
    DeferredCmdSetBlendColor(const Color &color)
        : DeferredCmdBase(RenderCommand::SetBlendColor)
    {
        color_ = color;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;

private:
    Color color_;
};

struct DeferredCmdSetDepthBias final : public DeferredCmdBase
{
    DeferredCmdSetDepthBias(float depthBiasConstantFactor,
        float depthBiasClamp,
        float depthBiasSlopeFactor)
        : DeferredCmdBase(RenderCommand::SetDepthBias)
    {
        depthBiasConstantFactor_ = depthBiasConstantFactor;
        depthBiasClamp_ = depthBiasClamp;
        depthBiasSlopeFactor_ = depthBiasSlopeFactor;
    }

    virtual void Execute(VKCommandBuffer* commandBuffer) override;

private:
    float                                       depthBiasConstantFactor_;
    float                                       depthBiasClamp_;
    float                                       depthBiasSlopeFactor_;
};

struct DeferredCmdPushDebugGroup final : public DeferredCmdBase
{
    DeferredCmdPushDebugGroup(const std::string &groupLabel)
        : DeferredCmdBase(RenderCommand::PushDebugGroup)
    {
        size_t length = std::min(groupLabel.size(), strings_.size() - 1);
        memcpy(strings_.data(), groupLabel.c_str(), length);
        strings_[length] = 0;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    std::array<char, 64> strings_;
};

struct DeferredCmdPopDebugGroup final : public DeferredCmdBase
{
    DeferredCmdPopDebugGroup()
        : DeferredCmdBase(RenderCommand::PopDebugGroup)
    {
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
};

struct DeferredCmdInsertDebugMarker final : public DeferredCmdBase
{
    DeferredCmdInsertDebugMarker(const std::string &markerLabel)
        : DeferredCmdBase(RenderCommand::InsertDebugMarker)
    {
        size_t length = std::min(markerLabel.size(), strings_.size() - 1);
        memcpy(strings_.data(), markerLabel.c_str(), length);
        strings_[length] = 0;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    std::array<char, 64> strings_;
};

struct DeferredCmdBeginOcclusionQuery final : public DeferredCmdBase
{
    DeferredCmdBeginOcclusionQuery(std::uint32_t queryIndex)
        : DeferredCmdBase(RenderCommand::BeginOcclusionQuery), queryIndex_(queryIndex)
    {
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    std::uint32_t queryIndex_;
};

struct DeferredCmdEndOcclusionQuery final : public DeferredCmdBase
{
    DeferredCmdEndOcclusionQuery(std::uint32_t queryIndex)
        : DeferredCmdBase(RenderCommand::EndOcclusionQuery), queryIndex_(queryIndex)
    {
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    std::uint32_t queryIndex_;
};

struct DeferredCmdResolveQuerySet final : public DeferredCmdBase
{
    DeferredCmdResolveQuerySet(const QuerySetPtr& querySet,
                               std::uint32_t queryFirstIndex,
                               std::uint32_t queryCount,
                               const BufferPtr& dstBuffer,
                               std::uint32_t dstOffset)
        : DeferredCmdBase(RenderCommand::ResolveQuerySet)
    {
        querySet_ = querySet;
        queryFirstIndex_ = queryFirstIndex;
        queryCount_ = queryCount;
        dstBuffer_ = dstBuffer;
        dstOffset_ = dstOffset;
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    QuerySetPtr querySet_;
    std::uint32_t queryFirstIndex_;
    std::uint32_t queryCount_;
    BufferPtr dstBuffer_;
    std::uint32_t dstOffset_;
};

struct DeferredCmdPipelineBarrier final : public DeferredCmdBase
{
    DeferredCmdPipelineBarrier(TexturePtr texture,
                               TextureUsageFlags srcUsageFlags,
                               TextureUsageFlags dstUsageFlags)
        : DeferredCmdBase(RenderCommand::PipelineBarrier)
        , texture_(texture)
        , srcUsageFlags_(srcUsageFlags)
        , dstUsageFlags_(dstUsageFlags)
    {
    }
    
    virtual void Execute(VKCommandBuffer* commandBuffer) override;
    
private:
    TexturePtr texture_ = nullptr;
    TextureUsageFlags srcUsageFlags_;
    TextureUsageFlags dstUsageFlags_;
};

NS_GFX_END

#endif //PROJ_ANDROID_DEFERREDRENDERCOMMANDS_H
