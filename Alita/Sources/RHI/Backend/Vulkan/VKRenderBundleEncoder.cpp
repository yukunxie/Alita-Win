//
// Created by realxie on 2020-01-22.
//

#include "VKRenderBundleEncoder.h"
#include "VKBindGroup.h"
#include "VKRenderBundle.h"

NS_RHI_BEGIN

VKRenderBundleEncoder::VKRenderBundleEncoder(VKDevice* device)
    : RenderBundleEncoder(device)
{
}

VKRenderBundleEncoder::~VKRenderBundleEncoder()
{
    Dispose();
}

void VKRenderBundleEncoder::Dispose()
{
    RHI_DISPOSE_BEGIN();
    
    RHI_SAFE_RELEASE(renderBundle_);
    
    RHI_DISPOSE_END();
}

bool VKRenderBundleEncoder::Init(const RenderBundleEncoderDescriptor &descriptor)
{
    // TODO realxie : descriptor ????
    RHI_PTR_ASSIGN(renderBundle_, VKDEVICE()->CreateObject<VKRenderBundle>());
    return nullptr != renderBundle_;
}

void VKRenderBundleEncoder::CheckRenderBundle()
{
    if (!renderBundle_)
    {
        renderBundle_ = VKDEVICE()->CreateObject<VKRenderBundle>();
    }
}

void VKRenderBundleEncoder::SetPipeline(RenderPipeline* pipeline)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdSetRenderPipeline>(pipeline);
}

void VKRenderBundleEncoder::SetIndexBuffer(Buffer* buffer, std::uint32_t offset)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdSetIndexBuffer>(buffer, offset);
}

void VKRenderBundleEncoder::SetVertexBuffer(Buffer* buffer, std::uint32_t offset,
                                            std::uint32_t slot)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdSetVertexBuffer>(buffer, offset, slot);
}

void VKRenderBundleEncoder::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount,
                                 std::uint32_t firstVertex,
                                 std::uint32_t firstInstance)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdDrawArrays>(vertexCount, instanceCount, firstVertex,
                                                 firstInstance);
}

void VKRenderBundleEncoder::Draw(std::uint32_t vertexCount, std::uint32_t firstVertex)
{
    Draw(vertexCount, 1, firstVertex, 0);
}

void VKRenderBundleEncoder::DrawIndxed(std::uint32_t indexCount, std::uint32_t firstIndex)
{
    DrawIndexed(indexCount, 1, firstIndex, 0, 0);
}

void VKRenderBundleEncoder::DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                                        std::uint32_t firstIndex, int32_t baseVertex,
                                        std::uint32_t firstInstance)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdDrawIndexed>(indexCount, instanceCount, firstIndex,
                                                  baseVertex, firstInstance);
}

void VKRenderBundleEncoder::DrawIndirect(Buffer* indirectBuffer, BufferSize indirectOffset)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdDrawIndirect>(indirectBuffer, indirectOffset);
}

void VKRenderBundleEncoder::DrawIndexedIndirect(Buffer* indirectBuffer, BufferSize indirectOffset)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdDrawIndexedIndirect>(indirectBuffer, indirectOffset);
}

void VKRenderBundleEncoder::SetViewport(float x, float y, float width, float height,
                                        float minDepth, float maxDepth)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdSetViewport>(x, y, width, height, minDepth, maxDepth);
}

void
VKRenderBundleEncoder::SetScissorRect(std::int32_t x, std::int32_t y, std::uint32_t width,
                                      std::uint32_t height)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdSetScissorRect>(x, y, width, height);
}

void
VKRenderBundleEncoder::SetBindGroup(std::uint32_t index, BindGroup* bindGroup, std::uint32_t count,
                                    const uint32_t* dynamicOffsets)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdSetBindGroup>(index, bindGroup, count, dynamicOffsets);
}

void VKRenderBundleEncoder::PushDebugGroup(const std::string &groupLabel)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdPushDebugGroup>(groupLabel);
}

void VKRenderBundleEncoder::PopDebugGroup()
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdPopDebugGroup>();
}

void VKRenderBundleEncoder::InsertDebugMarker(const std::string &markerLabel)
{
    // CheckRenderBundle();
    renderBundle_->RecoardCommand<CmdInsertDebugMarker>(markerLabel);
}

RenderBundle* VKRenderBundleEncoder::Finish(const RenderBundleDescriptor &descriptor)
{
    RHI_ASSERT(renderBundle_, "renderBundle_ must not be; nullptr.");
    renderBundle_->AutoRelease();
    auto tmp = renderBundle_;
    renderBundle_ = nullptr;
    return tmp;
}

void VKRenderBundleEncoder::Reset()
{
    auto tmp = VKDEVICE()->CreateObject<VKRenderBundle>();
    RHI_PTR_ASSIGN(renderBundle_, tmp);
}

NS_RHI_END