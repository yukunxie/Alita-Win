//
// Created by realxie on 2019-10-14.
//

#include "VKCommandBuffer.h"
#include "VKBindGroup.h"
#include "VKRenderPipeline.h"
#include "VKComputePipeline.h"
#include "VKBuffer.h"
#include "VKRenderPass.h"
#include "VKFramebuffer.h"
#include "VKTexture.h"
#include "RenderThreading.h"
#include "VKSampler.h"
#include "VKTextureView.h"
#include "VKQuerySet.h"

NS_GFX_BEGIN

RenderPassEntry::RenderPassEntry(RenderPassPtr renderPass,
                                 FramebufferPtr framebuffer,
                                 std::uint32_t clearValueCount,
                                 Color* clearValues,
                                 float clearDepth,
                                 uint32_t clearStencil)
{
    char buffer[2048];
    size_t offset = 0;
    char* header = buffer;
    const VkRenderPass vkRenderPass = GFX_CAST(VKRenderPass*, renderPass)->GetNative();
    memcpy(header, &vkRenderPass, sizeof(vkRenderPass));
    header = header + sizeof(vkRenderPass);
    
    const VkFramebuffer vkFramebuffer = GFX_CAST(VKFramebuffer*, framebuffer)->GetNative();
    memcpy(header, &vkFramebuffer, sizeof(vkFramebuffer));
    header = header + sizeof(vkFramebuffer);
    
    memcpy(header, clearValues, sizeof(Color) * clearValueCount);
    header = header + sizeof(Color) * clearValueCount;
    
    memcpy(header, &clearDepth, sizeof(float));
    header = header + sizeof(float);
    
    memcpy(header, &clearStencil, sizeof(uint32_t));
    header = header + sizeof(uint32_t);
    
    static XXHash64 sHash64(0x238923);
    hash_ = sHash64.hash(buffer, header - buffer, 0x77654899);
}


VKCommandBuffer::VKCommandBuffer(DevicePtr device)
    : CommandBuffer(device)
{
}

bool VKCommandBuffer::Init()
{
    return true;
}

void VKCommandBuffer::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    ResetCommandBuffer();
    
    GFX_DISPOSE_END();
}

void VKCommandBuffer::BeginRenderPass(RenderPassPtr pass,
                                      FramebufferPtr framebuffer,
                                      QuerySetPtr occlusionQuerySet,
                                      std::uint32_t clearValueCount,
                                      const Color* clearValues,
                                      float clearDepth,
                                      uint32_t clearStencil)
{
    VKRenderPass* vkRenderPass = GFX_CAST(VKRenderPass*, pass);
    VKFramebuffer* vkFramebuffer = GFX_CAST(VKFramebuffer*, framebuffer);
    
    if (renderPassBinding_ && framebufferBinding_)
    {
        if (GFX_CAST(VKRenderPass*, renderPassBinding_)->GetNative() == vkRenderPass->GetNative() &&
            GFX_CAST(VKFramebuffer*, framebufferBinding_)->GetNative() == vkFramebuffer->GetNative())
        {
            return;
        }
    }
    
    ForceEndRenderPass();
    
    renderPassBinding_ = pass;
    framebufferBinding_ = framebuffer;
    occlusionQuerySetBinding_ = occlusionQuerySet;
    
    VkOffset2D offset = {0, 0};
    const auto &fbSize = GFX_CAST(VKFramebuffer*, framebufferBinding_)->GetExtent2D();
    VkExtent2D extent = {fbSize.width, fbSize.height};
    
    VkRenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = GFX_CAST(VKRenderPass*, renderPassBinding_)->GetNative();
    renderPassBeginInfo.framebuffer = GFX_CAST(VKFramebuffer*, framebufferBinding_)->GetNative();
    renderPassBeginInfo.renderArea.offset = offset;
    renderPassBeginInfo.renderArea.extent = extent;
    
    std::array<VkClearValue, kMaxColorAttachments + 1> vkClearValues;
    GFX_ASSERT(clearValueCount + 1 <= vkClearValues.size());
    for (std::uint32_t i = 0; i < clearValueCount; ++i)
    {
        auto &value = clearValues[i];
        vkClearValues[i] = {value.r, value.g, value.b, value.a};
    }
    
    vkClearValues[clearValueCount].depthStencil.depth = clearDepth;
    vkClearValues[clearValueCount].depthStencil.stencil = clearStencil;
    
    renderPassBeginInfo.clearValueCount = clearValueCount + 1;
    renderPassBeginInfo.pClearValues = vkClearValues.data();
    
    if (occlusionQuerySet && occlusionQuerySet->GetCount())
    {
        // vkCmdResetQueryPool must only be called outside of a render pass instance
        vkCmdResetQueryPool(GetNative(), GFX_CAST(VKQuerySet*, occlusionQuerySet)->GetNative(), 0, occlusionQuerySet->GetCount());
    }
    
    {
        // transform image layout to optimal image layout
        for (size_t i = 0; i < vkFramebuffer->GetColorAttachmentCount(); ++i)
        {
            auto textureView = vkFramebuffer->GetColorAttachments()[i];
            auto texture = GFX_CAST(VKTextureView*, textureView)->GetTexture();
            GFX_CAST(VKTexture*, texture)->TransToOutputAttachmentImageLayout(this);
        }
        GFX_ASSERT(currentRenderPassIndex_ < bindGroupCatagories_.size());
        std::vector<BindGroupPtr>& bindGroups = bindGroupCatagories_[currentRenderPassIndex_++];
        for (BindGroupPtr pBindGroup : bindGroups)
        {
            GFX_CAST(VKBindGroup*, pBindGroup)->TransImageLayoutToSampled(this);
        }
    }

    vkCmdBeginRenderPass(GetNative(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    // 这里需要判断一个是否是对swapchain中的image进行操作的
    isRenderPassPresentable_ = GFX_CAST(VKFramebuffer*, framebufferBinding_)->HasSwapChainImages();
}

void VKCommandBuffer::ClearAttachment(VkClearAttachment clearAttachment)
{
    // TODO realxie, 这种操作具有比较大的性能损失，并不能代替renderpass中的clear操作
    // GFX_ASSERT(framebufferBinding_);
    //
    // VkOffset2D offset;
    // offset.x = 0;
    // offset.y = 0;
    //
    // VkExtent2D extent;
    // extent.width = framebufferBinding_->GetExtent2D().width;
    // extent.height = framebufferBinding_->GetExtent2D().height;
    //
    // VkClearRect clearRect;
    // clearRect.rect = {offset, extent};
    // clearRect.baseArrayLayer = 0;
    // clearRect.layerCount = 1;
    //
    // vkCmdClearAttachments(vkCommandBuffer_, 1, &clearAttachment, 1, &clearRect);
}

void VKCommandBuffer::EndRenderPass()
{
    bHasPendingEndRenderPass_ = true;
    ForceEndRenderPass();
}

template<typename BasePipelineType>
void SetBindGroupToGraphicPipelineImpl(VKCommandBuffer* thiz,
    BasePipelineType pipeline,
    std::uint32_t index,
    BindGroupPtr bindGroup,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets)
{
    auto bindGroupLayout = GFX_CAST(VKBindGroup*, bindGroup)->GetBindGroupLayout();
    auto expectedOffsetCount = bindGroupLayout->GetDynamicOffsetCount();

    auto vkDescriptorSet = thiz->AsyncWriteBindGroupToGPU(GFX_CAST(VKBindGroup*, bindGroup));

    if (dynamicOffsetCount >= expectedOffsetCount)
    {
        vkCmdBindDescriptorSets(thiz->GetNative(),
            pipeline->GetPipelineBindPoint(),
            pipeline->GetPipelineLayout(),
            index,
            1,
            &vkDescriptorSet,
            dynamicOffsetCount,
            pDynamicOffsets);
    }
    else
    {
        std::vector<std::uint32_t> offsets(expectedOffsetCount, 0);
        for (std::uint32_t i = 0; i < dynamicOffsetCount; ++i)
        {
            offsets[i] = pDynamicOffsets[i];
        }
        vkCmdBindDescriptorSets(thiz->GetNative(),
            pipeline->GetPipelineBindPoint(),
            pipeline->GetPipelineLayout(),
            index,
            1,
            &vkDescriptorSet,
            (std::uint32_t)offsets.size(),
            offsets.data());
    }
}

void VKCommandBuffer::SetBindGroupToGraphicPipeline(std::uint32_t index, BindGroupPtr bindGroup,
                                                    uint32_t dynamicOffsetCount,
                                                    const uint32_t* pDynamicOffsets)
{
    GFX_ASSERT(graphicPipelineBinding_, "graphicPipelineBinding_ can't be NULL");
    SetBindGroupToGraphicPipelineImpl(this,
        GFX_CAST(VKRenderPipeline*, graphicPipelineBinding_),
        index,
        bindGroup,
        dynamicOffsetCount,
        pDynamicOffsets);
}

void VKCommandBuffer::SetBindGroupToComputePipeline(std::uint32_t index, BindGroupPtr bindGroup,
                                                    uint32_t dynamicOffsetCount,
                                                    const uint32_t* pDynamicOffsets)
{
    GFX_ASSERT(computePipelineBinding_, "computePipelineBinding_ can't be NULL");
    SetBindGroupToGraphicPipelineImpl(this,
        GFX_CAST(VKComputePipeline*, computePipelineBinding_),
        index,
        bindGroup,
        dynamicOffsetCount,
        pDynamicOffsets);
}


void VKCommandBuffer::BindGraphicsPipeline(RenderPipelinePtr graphicPipeline)
{
    if (graphicPipelineBinding_ == graphicPipeline)
    {
        return;
    }
    graphicPipelineBinding_ = graphicPipeline;
    vkCmdBindPipeline(GetNative(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                        GFX_CAST(VKRenderPipeline *, graphicPipeline)->GetNative());
}

void VKCommandBuffer::BindComputePipeline(ComputePipelinePtr computePipeline)
{
    if (computePipelineBinding_ == computePipeline)
    {
        return;
    }
    computePipelineBinding_ = computePipeline;
    
    vkCmdBindPipeline(GetNative(), VK_PIPELINE_BIND_POINT_COMPUTE,
                        GFX_CAST(VKComputePipeline *, computePipeline)->GetNative());
}

void VKCommandBuffer::SetIndexBuffer(BufferPtr buffer, std::uint32_t offset)
{
    // The format of indexBuffer depends on the GraphicPipeline, so, we need to delay set the index buffer
    // by SetIndexBufferInternal
    boundIndexBufferItem_.indexBuffer = buffer;
    boundIndexBufferItem_.offset = offset;
}

void
VKCommandBuffer::SetVertexBuffer(BufferPtr buffer, std::uint32_t offset, std::uint32_t slot)
{
    auto vkBuffer = GFX_CAST(VKBuffer *, buffer)->GetNative();
    VkDeviceSize vkOffset = offset;
    vkCmdBindVertexBuffers(GetNative(), slot, 1, &vkBuffer, &vkOffset);
}

void VKCommandBuffer::Dispatch(std::uint32_t groupCountX, std::uint32_t groupCountY,
                               std::uint32_t groupCountZ)
{
    vkCmdDispatch(GetNative(), groupCountX, groupCountY, groupCountZ);
}

void VKCommandBuffer::DispatchIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset)
{
    vkCmdDispatchIndirect(GetNative(), GFX_CAST(VKBuffer *, indirectBuffer)->GetNative(),
                            indirectOffset);
}

void VKCommandBuffer::AddBindingBuffer(const BufferPtr& buffer)
{
    buffer->increaseBindCount();
    bindBuffers_.push_back(buffer);
}

void VKCommandBuffer::AddBindingBindGroup(const BindGroupPtr& bindGroup)
{
    if (isInRenderPass_ && (bindGroup->GetObjectType() == RHIObjectType::BindGroup))
    {
        bindGroupCatagories_.back().push_back(bindGroup);

        const std::vector<BufferPtr>& buffs = GFX_CAST(VKBindGroup*, bindGroup)->getBindingBuffers();
        for (auto buff : buffs)
        {
            buff->increaseBindCount();
            bindBuffers_.push_back(buff);
        }
    }
}

void VKCommandBuffer::ResetCommandBuffer()
{
    // if (enableCalcBindBufferCount_)
    // {
    for (auto buffer : bindBuffers_)
    {
        if (buffer)
            buffer->reduceBindCount();
    }
    bindBuffers_.clear();
    // }

    bindGroupCatagories_.clear();
    isRecording_ = false;
    currentRenderPassIndex_ = 0;
    
    graphicPipelineBinding_ = nullptr;
    computePipelineBinding_ = nullptr;
    
    //if (pCommandList_)
    //{
    //    VKDEVICE()->ReleaseCommandList(pCommandList_);
    //    pCommandList_.reset();
    //}

    //GFX_ASSERT(pCommandList_->GetCmdCount() == 0);
    ResetCommandList();
    
    vkCommandBuffer_ = VK_NULL_HANDLE;
}

void VKCommandBuffer::BeginCommandBuffer()
{
    if (isRecording_) return;
    isRecording_ = true;
    hasSetViewport_ = false;
    hasSetScissor_ = false;
    
    if (vkCommandBuffer_ != VK_NULL_HANDLE)
    {
        vkResetCommandBuffer(vkCommandBuffer_, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }
    else
    {
        vkCommandBuffer_ = GET_THREAD_LOCAL_ALLOCATOR()->AcquireVkCommandBuffer();
    }
    
    VkCommandBufferBeginInfo cmdBufferBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr
    };
    CALL_VK(vkBeginCommandBuffer(vkCommandBuffer_, &cmdBufferBeginInfo));
}

void VKCommandBuffer::EndCommandBuffer()
{
    ForceEndRenderPass();
    CALL_VK(vkEndCommandBuffer(vkCommandBuffer_));
    
    isRecording_ = false;
    graphicPipelineBinding_ = nullptr;
    computePipelineBinding_ = nullptr;
}

VKCommandBuffer::~VKCommandBuffer()
{
    Dispose();
}

extern PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBegin;

void VKCommandBuffer::PushDebugGroup(const char* groupLabel)
{
    if (! vkCmdDebugMarkerBegin|| !VKDEVICE()->SupportDebugGroup())
    {
        return;
    }
    
    VkDebugMarkerMarkerInfoEXT makeInfo;
    makeInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
    makeInfo.pNext = nullptr;
    
    static uint32_t counter = 0;
    if (((counter++) & 1) == 0)
    {
        makeInfo.color[0] = 0.25f;
        makeInfo.color[1] = 0.0f;
        makeInfo.color[2] = 0.47f;
        makeInfo.color[3] = 1.0f;
    }
    else
    {
        makeInfo.color[0] = 0.11f;
        makeInfo.color[1] = 0.47f;
        makeInfo.color[2] = 0.43f;
        makeInfo.color[3] = 1.0f;
    }
    makeInfo.pMarkerName = groupLabel;
    
    vkCmdDebugMarkerBegin(GetNative(), &makeInfo);
}

extern PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEnd;

void VKCommandBuffer::PopDebugGroup()
{
    if (!vkCmdDebugMarkerEnd || !VKDEVICE()->SupportDebugGroup())
    {
        return;
    }
    vkCmdDebugMarkerEnd(GetNative());
}

extern PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsert;

void VKCommandBuffer::InsertDebugMarker(const char* markerLabel)
{
    if (!vkCmdDebugMarkerInsert || !VKDEVICE()->SupportDebugGroup())
    {
        return;
    }
    
    VkDebugMarkerMarkerInfoEXT makeInfo;
    makeInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
    makeInfo.pNext = nullptr;
    makeInfo.color[0] = 0.0f;
    makeInfo.color[1] = 1.0f;
    makeInfo.color[2] = 1.0f;
    makeInfo.color[3] = 1.0f;
    makeInfo.pMarkerName = markerLabel;
    
    vkCmdDebugMarkerInsert(GetNative(), &makeInfo);
}

void VKCommandBuffer::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount,
                           std::uint32_t firstVertex, std::uint32_t firstInstance)
{
    CheckViewportScissorBeforDraw();
    
    vkCmdDraw(GetNative(),
                vertexCount,
                instanceCount,
                firstVertex,
                firstInstance);
}

void VKCommandBuffer::Draw(std::uint32_t vertexCount, std::uint32_t firstVertex)
{
    this->Draw(vertexCount,
               1,
               firstVertex,
               0);
}

void VKCommandBuffer::DrawIndxed(std::uint32_t indexCount, std::uint32_t firstIndex)
{
    DrawIndexed(indexCount,
                1,
                firstIndex,
                0,
                0);
}

void VKCommandBuffer::DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount,
                                  std::uint32_t firstIndex, int32_t baseVertex,
                                  std::uint32_t firstInstance)
{
    SetIndexBufferInternal();
    
    CheckViewportScissorBeforDraw();
    
    vkCmdDrawIndexed(GetNative(),
                       indexCount,
                       instanceCount,
                       firstIndex,
                       baseVertex,
                       firstInstance);
}

void VKCommandBuffer::DrawIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset)
{
    CheckViewportScissorBeforDraw();
    
    BufferSize bufferSize = indirectBuffer->GetBufferSize();
    
    GFX_ASSERT(bufferSize > indirectOffset);
    std::uint32_t stride = sizeof(VkDrawIndirectCommand);
    std::uint32_t drawCount = (bufferSize - indirectOffset) / stride;
    
    vkCmdDrawIndirect(GetNative(),
                        GFX_CAST(VKBuffer *, indirectBuffer)->GetNative(),
                        indirectOffset,
                        drawCount,
                        stride);
}

void VKCommandBuffer::DrawIndexedIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset)
{
    SetIndexBufferInternal();
    
    CheckViewportScissorBeforDraw();
    
    BufferSize bufferSize = indirectBuffer->GetBufferSize();
    
    GFX_ASSERT(bufferSize > indirectOffset);
    std::uint32_t stride = sizeof(VkDrawIndexedIndirectCommand);
    std::uint32_t drawCount = (bufferSize - indirectOffset) / stride;
    
    vkCmdDrawIndexedIndirect(GetNative(),
                               GFX_CAST(VKBuffer *, indirectBuffer)->GetNative(),
                               indirectOffset,
                               drawCount,
                               stride);
}

void VKCommandBuffer::SetViewport(float x, float y, float width, float height,
                                  float minDepth,
                                  float maxDepth)
{
    hasSetViewport_ = true;
    
    if (VKDEVICE()->IsSupportNegativeViewport())
    {
        // Using a negative height to flip the viewport.
        VkViewport viewport = {x, y + height, width, -height, minDepth, maxDepth};
        vkCmdSetViewport(GetNative(), 0, 1, &viewport);
    }
    else
    {
        // Viewport的翻转使用gl_Position.y *= -1来进行处理
        VkViewport viewport = {x, y, width, height, minDepth, maxDepth};
        vkCmdSetViewport(GetNative(), 0, 1, &viewport);
    }
}

void VKCommandBuffer::SetScissorRect(uint32_t x, uint32_t y, std::uint32_t width,
                                     std::uint32_t height)
{
    hasSetScissor_ = true;
    
    VkRect2D scissor = {{(int32_t) x, (int32_t) y},
                        {width,       height}};
    vkCmdSetScissor(GetNative(), 0, 1, &scissor);
}

void VKCommandBuffer::SetStencilReference(std::uint32_t reference)
{
    vkCmdSetStencilReference(GetNative(), VK_STENCIL_FRONT_AND_BACK, reference);
}

void VKCommandBuffer::SetBlendColor(const Color &color)
{
    vkCmdSetBlendConstants(GetNative(), &color.r);
}

void VKCommandBuffer::SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    vkCmdSetDepthBias(GetNative(), depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void VKCommandBuffer::BeginOcclusionQuery(std::uint32_t queryIndex)
{
    GFX_ASSERT(occlusionQuerySetBinding_);
    vkCmdBeginQuery(GetNative(),
        GFX_CAST(VKQuerySet*, occlusionQuerySetBinding_)->GetNative(),
        queryIndex,
        VK_QUERY_CONTROL_PRECISE_BIT);
}

void VKCommandBuffer::EndOcclusionQuery(std::uint32_t queryIndex)
{
    GFX_ASSERT(occlusionQuerySetBinding_);
    vkCmdEndQuery(GetNative(),
        GFX_CAST(VKQuerySet*, occlusionQuerySetBinding_)->GetNative(),
        queryIndex);
}

void VKCommandBuffer::ResolveQuerySet(
    QuerySetPtr querySet,
    std::uint32_t queryFirstIndex,
    std::uint32_t queryCount,
    BufferPtr dstBuffer,
    std::uint32_t dstOffset)
{
    vkCmdCopyQueryPoolResults(GetNative(),
                                GFX_CAST(VKQuerySet*, querySet)->GetNative(),
                                queryFirstIndex,
                                queryCount,
                                GFX_CAST(VKBuffer*, dstBuffer)->GetNative(),
                                dstOffset,
                                sizeof(std::uint32_t),
                                VK_QUERY_RESULT_WAIT_BIT);
}

void VKCommandBuffer::OnDeleteEvent() const
{
    VKDEVICE()->GetObjectManager().RemoveObjectFromCache(this);
}

void VKCommandBuffer::CheckViewportScissorBeforDraw()
{
    if (!hasSetViewport_)
    {
        const Viewport &vp = VKDEVICE()->GetDefaultViewport();
        SetViewport(vp.x, vp.y, vp.width, vp.height, vp.minDepth, vp.maxDepth);
    }
    
    if (!hasSetScissor_)
    {
        const Viewport &vp = VKDEVICE()->GetDefaultViewport();
        SetScissorRect(vp.x, vp.y, vp.width, vp.height);
    }
}

VkDescriptorSet VKCommandBuffer::AsyncWriteBindGroupToGPU(VKBindGroup* bindGroup)
{
#if defined(GFX_DEBUG) && GFX_DEBUG
    VKDEVICE()->GetAsyncWorker()->CheckThread();
#endif
    bindGroup->UpdateDescriptorSetAsync();
    return bindGroup->GetNative();
}

//void VKCommandBuffer::SubmitCommandList()
//{
//    if (!pCommandList_)
//    {
//        return;
//    }
//    
//    CommandListIterator iterator(pCommandList_.get());
//    while (iterator.HasCommandLeft())
//    {
//        auto cmd = iterator.NextCommand();
//#if 0
//        cmd->Execute(this);
//#else
//        switch (cmd->commandType)
//        {
//            case RenderCommand::BeginCommandBuffer:
//                static_cast<DeferredCmdBeginCommandBuffer*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::BeginRenderPass:
//                static_cast<DeferredCmdBeginRenderPass*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::ClearAttachment:
//                static_cast<DeferredCmdClearAttachment*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::CopyBufferToBuffer:
//                static_cast<DeferredCmdCopyBufferToBuffer*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::CopyBufferToTexture:
//                static_cast<DeferredCmdCopyBufferToTexture*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::CopyTextureToBuffer:
//                static_cast<DeferredCmdCopyTextureToBuffer*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::CopyTextureToTexture:
//                static_cast<DeferredCmdCopyTextureToTexture*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::Dispatch:
//                static_cast<DeferredCmdDispatch*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::DispatchIndirect:
//                static_cast<DeferredCmdDispatchIndirect*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::Draw:
//                static_cast<DeferredCmdDraw*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::DrawIndexed:
//                static_cast<DeferredCmdDrawIndexed*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::DrawIndirect:
//                static_cast<DeferredCmdDrawIndirect*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::DrawIndexedIndirect:
//                static_cast<DeferredCmdDrawIndexedIndirect*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::EndPass:
//                static_cast<DeferredCmdEndPass*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::SetComputePipeline:
//                static_cast<DeferredCmdSetComputePipeline*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::SetGraphicPipeline:
//                static_cast<DeferredCmdSetGraphicPipeline*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::SetStencilReference:
//                static_cast<DeferredCmdSetStencilReference*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::SetScissorRect:
//                static_cast<DeferredCmdSetScissorRect*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::SetViewport:
//                static_cast<DeferredCmdSetViewport*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::SetBlendColor:
//                static_cast<DeferredCmdSetBlendColor*>(cmd)->Execute(this);
//                break;
//
//            case RenderCommand::SetDepthBias:
//                static_cast<DeferredCmdSetDepthBias*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::SetBindGroup:
//                static_cast<DeferredCmdSetBindGroup*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::SetIndexBuffer:
//                static_cast<DeferredCmdSetIndexBuffer*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::SetVertexBuffer:
//                static_cast<DeferredCmdSetVertexBuffer*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::PushDebugGroup:
//                static_cast<DeferredCmdPushDebugGroup*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::PopDebugGroup:
//                static_cast<DeferredCmdPopDebugGroup*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::InsertDebugMarker:
//                static_cast<DeferredCmdInsertDebugMarker*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::BeginOcclusionQuery:
//                static_cast<DeferredCmdBeginOcclusionQuery*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::EndOcclusionQuery:
//                static_cast<DeferredCmdEndOcclusionQuery*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::ResolveQuerySet:
//                static_cast<DeferredCmdResolveQuerySet*>(cmd)->Execute(this);
//                break;
//    
//            case RenderCommand::PipelineBarrier:
//                static_cast<DeferredCmdPipelineBarrier*>(cmd)->Execute(this);
//                break;
//    
//            default:
//                LOGE("unprocessed command: %d", cmd->commandType);
//                GFX_ASSERT(false);
//        }
//#endif
//    }
//}

void VKCommandBuffer::SetIndexBufferInternal()
{
    if (!boundIndexBufferItem_.indexBuffer)
    {
        return;
    }
    
    if (!graphicPipelineBinding_)
    {
        LOGE("graphicPipelineBinding_ can't be nullptr!");
    }
    
    VkIndexType indexType = VK_INDEX_TYPE_UINT16;
    if (graphicPipelineBinding_->GetIndexFormat() == IndexFormat::UINT32)
    {
        indexType = VK_INDEX_TYPE_UINT32;
    }
    
    VkBuffer indexBuffer = GFX_CAST(VKBuffer*, boundIndexBufferItem_.indexBuffer)->GetNative();
    std::uint32_t offset = boundIndexBufferItem_.offset;
    vkCmdBindIndexBuffer(GetNative(), indexBuffer, offset, indexType);
    
    boundIndexBufferItem_.indexBuffer = nullptr;
    boundIndexBufferItem_.offset = 0;
}

bool VKCommandBuffer::BakeCmdBufferAsync(VkCommandBuffer vkCmdBuffer)
{
    isRecording_ = true;
    hasSetViewport_ = false;
    hasSetScissor_ = false;
    currentRenderPassIndex_ = 0;
    
    vkCommandBuffer_ = vkCmdBuffer;
    
    {
#if defined(GFX_DEBUG) && GFX_DEBUG
        char debugGroudTag[128];
        static uint32_t sFrameCount = 0;
        snprintf(debugGroudTag, sizeof(debugGroudTag), "%p", vkCommandBuffer_);
#else
        char debugGroudTag[128] = {0};
        static uint32_t sFrameCount = 0;
        /*snprintf(debugGroudTag, sizeof(debugGroudTag), "%p-%u", vkCommandBuffer_, pCommandList_->GetCmdCount());*/
#endif
        //PushDebugGroup(debugGroudTag);
        SubmitCommandList();
        //PopDebugGroup();
    }
    
    ForceEndRenderPass();
    
    isRecording_ = false;
    graphicPipelineBinding_ = nullptr;
    computePipelineBinding_ = nullptr;
    vkCommandBuffer_ = VK_NULL_HANDLE;
    
    return true;
}

void VKCommandBuffer::PostprocessCommandBuffer()
{
    ResetCommandBuffer();
    VKDEVICE()->GetObjectManager().AddObjectToCache(this);
}

void VKCommandBuffer::SetupTexturePipelineBarrier(VKCommandBuffer* commandBuffer, const TexturePtr& texture_, TextureUsageFlags srcUsageFlags_, TextureUsageFlags dstUsageFlags_)
{
    TextureFormat textureFormat = texture_->GetFormat();

    VkPipelineStageFlags srcStages = VulkanPipelineStage(srcUsageFlags_, textureFormat);
    VkPipelineStageFlags dstStages = VulkanPipelineStage(dstUsageFlags_, textureFormat);

    VkImageMemoryBarrier barrier;
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.srcAccessMask = VulkanAccessFlags(srcUsageFlags_, textureFormat);
    barrier.dstAccessMask = VulkanAccessFlags(dstUsageFlags_, textureFormat);
    barrier.oldLayout = GetVulkanImageLayout(srcUsageFlags_, textureFormat);
    barrier.newLayout = GetVulkanImageLayout(dstUsageFlags_, textureFormat);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = GFX_CAST(VKTexture*, texture_)->GetNative();

    // This transitions the whole resource but assumes it is a 2D texture
    barrier.subresourceRange.aspectMask = VulkanAspectMask(textureFormat);
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = texture_->GetMipLevelCount();
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = texture_->GetArrayLayerCount();

    auto vkCommandBuffer = commandBuffer->GetNative();
    vkCmdPipelineBarrier(vkCommandBuffer, srcStages, dstStages, 0, 0, NULL, 0, NULL, 1, &barrier);
}



void VKCommandBuffer::ForceEndRenderPass()
{
    if (bHasPendingEndRenderPass_ && renderPassBinding_ && framebufferBinding_)
    {
        vkCmdEndRenderPass(vkCommandBuffer_);
    }
    bHasPendingEndRenderPass_ = false;
    renderPassBinding_ = nullptr;
    framebufferBinding_ = nullptr;
    occlusionQuerySetBinding_ = nullptr;
}

NS_GFX_END