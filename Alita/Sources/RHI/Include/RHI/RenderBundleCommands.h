//
// Created by realxie on 2020-01-22.
//

#ifndef RHI_RENDERCOMMANDS_H
#define RHI_RENDERCOMMANDS_H

#include "Buffer.h"
#include "RenderPipeline.h"
#include "Descriptors.h"
#include "RenderPipeline.h"
#include "BindGroup.h"
#include "RenderPassEncoder.h"
#include "RenderCommandTypes.h"
#include "CommandBuffer.h"

NS_RHI_BEGIN


class RenderCommandBase
{
public:
    RenderCommandBase(RenderCommand commandType)
        : commandType(commandType)
    {
    }
    
    FORCE_INLINE RenderCommand GetCommandType()
    {
        return commandType;
    }
    
protected:
    ~RenderCommandBase() = default;
    
    // data member
    RenderCommand commandType;
};

class CmdSetRenderPipeline final : public RenderCommandBase
{
public:
    CmdSetRenderPipeline(RenderPipeline* pipeline)
        : RenderCommandBase(RenderCommand::SetGraphicPipeline)
    {
        RHI_PTR_ASSIGN(pipeline_, pipeline);
    }
    
    ~CmdSetRenderPipeline()
    {
        RHI_SAFE_RELEASE(pipeline_);
    }
    
    void Execute(CommandBuffer* commandBuffer);

protected:
    RenderPipeline* pipeline_ = nullptr;
};

class CmdSetViewport  final : public RenderCommandBase
{
public:
    CmdSetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
        : RenderCommandBase(RenderCommand::SetViewport)
    {
        viewport_.x = x;
        viewport_.y = y;
        viewport_.width = width;
        viewport_.height = height;
        viewport_.minDepth = minDepth;
        viewport_.maxDepth = maxDepth;
    }
    
    CmdSetViewport(const Viewport &viewport)
        : RenderCommandBase(RenderCommand::SetViewport), viewport_(viewport)
    {
    }
    
    ~CmdSetViewport()
    {
    }
    
    void Execute(CommandBuffer* commandBuffer);

protected:
    Viewport viewport_;
};


class CmdSetScissorRect  final : public RenderCommandBase
{
public:
    CmdSetScissorRect(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
        : RenderCommandBase(RenderCommand::SetScissorRect)
    {
        scissor_.x = x;
        scissor_.y = y;
        scissor_.width = width;
        scissor_.height = height;
    }
    
    CmdSetScissorRect(const Scissor &scissor)
        : RenderCommandBase(RenderCommand::SetScissorRect), scissor_(scissor)
    {
    }
    
    ~CmdSetScissorRect()
    {
    }
    
    void Execute(CommandBuffer* commandBuffer);

protected:
    Scissor scissor_;
};


class CmdSetBindGroup  final : public RenderCommandBase
{
public:
    CmdSetBindGroup(std::uint32_t index, BindGroup* bindGroup, std::uint32_t count,
                    const std::uint32_t* dynamicOffsets)
        : RenderCommandBase(RenderCommand::SetBindGroup), index_(index)
    {
        RHI_PTR_ASSIGN(bindGroup_, bindGroup);
        dynamicOffsets_.resize(count);
        for (std::uint32_t i = 0; i < count; ++i)
        {
            dynamicOffsets_[i] = dynamicOffsets[i];
        }
    }
    
    ~CmdSetBindGroup()
    {
        RHI_SAFE_RELEASE(bindGroup_);
    }
    
    void Execute(CommandBuffer* commandBuffer);

protected:
    std::uint32_t index_ = 0;
    BindGroup* bindGroup_ = nullptr;
    TurboVector<std::uint32_t, kMaxBindingsPerGroup> dynamicOffsets_;
};

class CmdSetVertexBuffer  final : public RenderCommandBase
{
public:
    CmdSetVertexBuffer(Buffer* buffer, std::uint32_t offset, std::uint32_t slot)
        : RenderCommandBase(RenderCommand::SetVertexBuffer), offset_(offset), slot_(slot)
    {
        RHI_PTR_ASSIGN(buffer_, buffer);
    }
    
    ~CmdSetVertexBuffer()
    {
        RHI_SAFE_RELEASE(buffer_);
    }
    
    void Execute(CommandBuffer* commandBuffer);

protected:
    Buffer* buffer_ = nullptr;
    std::uint32_t offset_ = 0;
    std::uint32_t slot_ = 0;
};

class CmdSetIndexBuffer  final : public RenderCommandBase
{
public:
    CmdSetIndexBuffer(Buffer* buffer, std::uint32_t offset)
        : RenderCommandBase(RenderCommand::SetIndexBuffer), offset_(offset)
    {
        RHI_PTR_ASSIGN(buffer_, buffer);
    }
    
    ~CmdSetIndexBuffer()
    {
        RHI_SAFE_RELEASE(buffer_);
    }
    
    void Execute(CommandBuffer* commandBuffer);

protected:
    Buffer* buffer_ = nullptr;
    std::uint32_t offset_ = 0;
};

class CmdDrawArrays  final : public RenderCommandBase
{
public:
    CmdDrawArrays(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex,
                  std::uint32_t firstInstance)
        : RenderCommandBase(RenderCommand::Draw),
          vertexCount(vertexCount), instanceCount(instanceCount),
          firstVertex(firstVertex), firstInstance(firstInstance)
    {
    }
    
    void Execute(CommandBuffer* commandBuffer);

public:
    std::uint32_t vertexCount;
    std::uint32_t instanceCount;
    std::uint32_t firstVertex;
    std::uint32_t firstInstance;
};

class CmdDrawIndexed  final : public RenderCommandBase
{
public:
    CmdDrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                   std::uint32_t firstIndex, int32_t baseVertex,
                   std::uint32_t firstInstance)
        : RenderCommandBase(RenderCommand::DrawIndexed),
          indexCount(indexCount), instanceCount(instanceCount),
          firstIndex(firstIndex), baseVertex(baseVertex),
          firstInstance(firstInstance)
    {
    }
    
    void Execute(CommandBuffer* commandBuffer);

public:
    std::uint32_t indexCount;
    std::uint32_t instanceCount;
    std::uint32_t firstIndex;
    std::uint32_t baseVertex;
    std::uint32_t firstInstance;
};

class CmdDrawIndirect  final : public RenderCommandBase
{
public:
    CmdDrawIndirect(Buffer* buffer_, BufferSize indirectOffset_)
        : RenderCommandBase(RenderCommand::DrawIndirect),
          indirectOffset(indirectOffset_)
    {
        RHI_PTR_ASSIGN(buffer, buffer_);
    }
    
    void Execute(CommandBuffer* commandBuffer);
    
    ~CmdDrawIndirect()
    {
        RHI_SAFE_RELEASE(buffer);
    }

public:
    Buffer* buffer = nullptr;
    BufferSize indirectOffset = 0;
};

class CmdDrawIndexedIndirect  final : public RenderCommandBase
{
public:
    CmdDrawIndexedIndirect(Buffer* buffer_, BufferSize indirectOffset_)
        : RenderCommandBase(RenderCommand::DrawIndexedIndirect),
          indirectOffset(indirectOffset_)
    {
        RHI_PTR_ASSIGN(buffer, buffer_);
    }
    
    void Execute(CommandBuffer* commandBuffer);
    
    ~CmdDrawIndexedIndirect()
    {
        RHI_SAFE_RELEASE(buffer);
    }

public:
    Buffer* buffer = nullptr;
    BufferSize indirectOffset = 0;
};

class CmdPushDebugGroup  final : public RenderCommandBase
{
public:
    CmdPushDebugGroup(const std::string& groupLabel)
        : RenderCommandBase(RenderCommand::PushDebugGroup)
        , groupLabel(groupLabel)
    {
    }
    
    ~CmdPushDebugGroup()
    {
    }
    
    void Execute(CommandBuffer* commandBuffer);

protected:
    std::string groupLabel;
};

class CmdPopDebugGroup  final : public RenderCommandBase
{
public:
    CmdPopDebugGroup()
        : RenderCommandBase(RenderCommand::PopDebugGroup)
    {
    }
    
    ~CmdPopDebugGroup()
    {
    }
    
    void Execute(CommandBuffer* commandBuffer);
};

class CmdInsertDebugMarker  final : public RenderCommandBase
{
public:
    CmdInsertDebugMarker(const std::string& markerLabel)
        : RenderCommandBase(RenderCommand::PushDebugGroup)
        , markerLabel(markerLabel)
    {
    }
    
    ~CmdInsertDebugMarker()
    {
    }
    
    void Execute(CommandBuffer* commandBuffer);

protected:
    std::string markerLabel;
};

NS_RHI_END

#endif //RHI_RENDERCOMMANDS_H
