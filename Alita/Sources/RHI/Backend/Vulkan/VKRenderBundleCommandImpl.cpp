//
// Created by realxie on 2020-03-03.
//

#include "VKCommandBuffer.h"
#include "GFX/RenderBundleCommands.h"

NS_GFX_BEGIN


void CmdSetRenderPipeline::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->BindGraphicsPipeline(pipeline_);
}

void CmdSetViewport::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->SetViewport(viewport_.x, viewport_.y,
                                                           viewport_.width, viewport_.height,
                                                           viewport_.minDepth, viewport_.maxDepth);
}

void CmdSetScissorRect::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->SetScissorRect(scissor_.x, scissor_.y,
                                                              scissor_.width, scissor_.height);
}

void CmdSetBindGroup::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->SetBindGroupToGraphicPipeline(index_, bindGroup_,
                                                                             dynamicOffsets_.size(),
                                                                             dynamicOffsets_.data());
}

void CmdSetVertexBuffer::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->SetVertexBuffer(buffer_, offset_, slot_);
}

void CmdSetIndexBuffer::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->SetIndexBuffer(buffer_, offset_);
}

void CmdDrawArrays::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->Draw(vertexCount, instanceCount, firstVertex,
                                                    firstInstance);
}

void CmdDrawIndexed::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->DrawIndexed(indexCount, instanceCount, firstIndex,
                                                           baseVertex, firstInstance);
}

void CmdDrawIndirect::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->DrawIndirect(buffer, indirectOffset);
}

void CmdDrawIndexedIndirect::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->DrawIndexedIndirect(buffer, indirectOffset);
}

void CmdPushDebugGroup::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->PushDebugGroup(groupLabel.c_str());
}

void CmdPopDebugGroup::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->PopDebugGroup();
}

void CmdInsertDebugMarker::Execute(CommandBuffer* commandBuffer)
{
    RHI_CAST(VKCommandBuffer*, commandBuffer)->InsertDebugMarker(markerLabel.c_str());
}

NS_GFX_END


