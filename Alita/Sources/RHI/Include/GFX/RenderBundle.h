//
// Created by realxie on 2020-01-22.
//

#ifndef RHI_RENDERBUNDLE_H
#define RHI_RENDERBUNDLE_H

#include "Macros.h"
#include "GFXBase.h"
#include "RenderPipeline.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "RenderBundleCommands.h"

NS_GFX_BEGIN

class RenderBundle : public GfxBase
{
public:
    RenderBundle(Device* GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::RenderBundle), buffers_(10240, 0)
    {
    }
    
    ~RenderBundle()
    {
        Dispose();
    }
    
    std::uint32_t CalcCmdAlignment(std::uint32_t pos)
    {
        // 8 bits alignment
        return (pos + 8 - 1) & (~(8 - 1));
    }
    
    virtual void Dispose() override
    {
        std::uint32_t idx = 0;
        while (idx < cmdOffset_)
        {
            RenderCommandBase* cmd = (RenderCommandBase*) &buffers_[idx];
            switch (cmd->GetCommandType())
            {
                case RenderCommand::Draw:
                    static_cast<CmdDrawArrays*>(cmd)->~CmdDrawArrays();
                    idx += sizeof(CmdDrawArrays);
                    break;
                case RenderCommand::DrawIndexed:
                    static_cast<CmdDrawIndexed*>(cmd)->~CmdDrawIndexed();
                    idx += sizeof(CmdDrawIndexed);
                    break;
                case RenderCommand::DrawIndirect:
                    static_cast<CmdDrawIndirect*>(cmd)->~CmdDrawIndirect();
                    idx += sizeof(CmdDrawIndirect);
                    break;
                case RenderCommand::DrawIndexedIndirect:
                    static_cast<CmdDrawIndexedIndirect*>(cmd)->~CmdDrawIndexedIndirect();
                    idx += sizeof(CmdDrawIndexedIndirect);
                    break;
                case RenderCommand::SetGraphicPipeline:
                    static_cast<CmdSetRenderPipeline*>(cmd)->~CmdSetRenderPipeline();
                    idx += sizeof(CmdSetRenderPipeline);
                    break;
                case RenderCommand::SetScissorRect:
                    static_cast<CmdSetScissorRect*>(cmd)->~CmdSetScissorRect();
                    idx += sizeof(CmdSetScissorRect);
                    break;
                case RenderCommand::SetViewport:
                    static_cast<CmdSetViewport*>(cmd)->~CmdSetViewport();
                    idx += sizeof(CmdSetViewport);
                    break;
                case RenderCommand::SetBindGroup:
                    static_cast<CmdSetBindGroup*>(cmd)->~CmdSetBindGroup();
                    idx += sizeof(CmdSetBindGroup);
                    break;
                case RenderCommand::SetIndexBuffer:
                    static_cast<CmdSetIndexBuffer*>(cmd)->~CmdSetIndexBuffer();
                    idx += sizeof(CmdSetIndexBuffer);
                    break;
                case RenderCommand::SetVertexBuffer:
                    static_cast<CmdSetVertexBuffer*>(cmd)->~CmdSetVertexBuffer();
                    idx += sizeof(CmdSetVertexBuffer);
                    break;
                case RenderCommand::PushDebugGroup:
                    static_cast<CmdPushDebugGroup*>(cmd)->~CmdPushDebugGroup();
                    idx += sizeof(CmdPushDebugGroup);
                    break;
                case RenderCommand::PopDebugGroup:
                    static_cast<CmdPopDebugGroup*>(cmd)->~CmdPopDebugGroup();
                    idx += sizeof(CmdPopDebugGroup);
                    break;
                case RenderCommand::InsertDebugMarker:
                    static_cast<CmdInsertDebugMarker*>(cmd)->~CmdInsertDebugMarker();
                    idx += sizeof(CmdInsertDebugMarker);
                    break;
                default:
                    RHI_ASSERT(false);
            }
            
            idx = CalcCmdAlignment(idx);
        }
        cmdOffset_ = 0;
    }
    
    template<typename RenderCommandName, typename ...ArgTypes>
    void RecoardCommand(ArgTypes ... args)
    {
        if (cmdOffset_ + sizeof(RenderCommandName) > buffers_.size())
        {
            buffers_.resize(std::max(buffers_.size() * 2, (size_t) 1024));
        }
        
        auto cmd = new((void*) &buffers_[cmdOffset_])RenderCommandName(args...);
        cmdOffset_ = CalcCmdAlignment(cmdOffset_ + sizeof(RenderCommandName));
    }
    
    void Execute(CommandBuffer* commandBuffer)
    {
        std::uint32_t idx = 0;
        while (idx < cmdOffset_)
        {
            RenderCommandBase* cmd = (RenderCommandBase*) &buffers_[idx];
            switch (cmd->GetCommandType())
            {
                case RenderCommand::Draw:
                    static_cast<CmdDrawArrays*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdDrawArrays);
                    break;
                case RenderCommand::DrawIndexed:
                    static_cast<CmdDrawIndexed*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdDrawIndexed);
                    break;
                case RenderCommand::DrawIndirect:
                    static_cast<CmdDrawIndirect*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdDrawIndirect);
                    break;
                case RenderCommand::DrawIndexedIndirect:
                    static_cast<CmdDrawIndexedIndirect*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdDrawIndexedIndirect);
                    break;
                case RenderCommand::SetGraphicPipeline:
                    static_cast<CmdSetRenderPipeline*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdSetRenderPipeline);
                    break;
                case RenderCommand::SetScissorRect:
                    static_cast<CmdSetScissorRect*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdSetScissorRect);
                    break;
                case RenderCommand::SetViewport:
                    static_cast<CmdSetViewport*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdSetViewport);
                    break;
                case RenderCommand::SetBindGroup:
                    static_cast<CmdSetBindGroup*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdSetBindGroup);
                    break;
                case RenderCommand::SetIndexBuffer:
                    static_cast<CmdSetIndexBuffer*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdSetIndexBuffer);
                    break;
                case RenderCommand::SetVertexBuffer:
                    static_cast<CmdSetVertexBuffer*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdSetVertexBuffer);
                    break;
                case RenderCommand::PushDebugGroup:
                    static_cast<CmdPushDebugGroup*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdPushDebugGroup);
                    break;
                case RenderCommand::PopDebugGroup:
                    static_cast<CmdPopDebugGroup*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdPopDebugGroup);
                    break;
                case RenderCommand::InsertDebugMarker:
                    static_cast<CmdInsertDebugMarker*>(cmd)->Execute(commandBuffer);
                    idx += sizeof(CmdInsertDebugMarker);
                    break;
                
                default:
                    RHI_ASSERT(false);
            }
    
            idx = CalcCmdAlignment(idx);
        }
    }

protected:
    std::vector<std::uint8_t> buffers_;
    std::uint32_t cmdOffset_ = 0;
};

NS_GFX_END

#endif //RHI_RENDERBUNDLE_H
