//
// Created by realxie on 2019-10-14.
//

#ifndef RHI_VKCOMMANDBUFFER_H
#define RHI_VKCOMMANDBUFFER_H

#include "VKDevice.h"
#include "VKBindGroupLayout.h"
#include "DeferredRenderCommands.h"
#include "CommandList.h"
#include "VKRenderPass.h"
#include "VKFramebuffer.h"
#include "VKQuerySet.h"

NS_GFX_BEGIN

class VKBindGroup;

class VKCommandBuffer;

struct RenderPassEntry
{
    RenderPassEntry(RenderPassPtr renderPass,
                    FramebufferPtr framebuffer,
                    std::uint32_t clearValueCount,
                    Color* clearValues,
                    float clearDepth,
                    uint32_t clearStencil);
    
    void Reset()
    {
        hash_ = 0;
        clearDepth = std::numeric_limits<float>::min();
    }

protected:
    std::uint64_t hash_ = 0;
    float clearDepth = std::numeric_limits<float>::min();
};

class VKCommandBuffer final : public CommandBuffer
{
protected:
    VKCommandBuffer(DevicePtr device);
    
public:
    virtual ~VKCommandBuffer();

public:
    
    bool Init();
    
    VkCommandBuffer GetNative() const
    { return vkCommandBuffer_; }
    
    template<typename CommandName, typename ...ArgTypes>
    void RecordCommand(ArgTypes ... args)
    {
        pCommandList_ = pCommandList_? pCommandList_ : VKDEVICE()->GetCommandList();
        pCommandList_->RecordCommand<CommandName>(args...);
    }
    
    virtual bool HasDrawCmdInPresentableImage() override
    { return isRenderPassPresentable_; }
    
    void BeginRenderPassInRecordingStage()
    {
        // 在一个新的pass中记录绑定的bindGroup
        isInRenderPass_ = true;
        bindGroupCatagories_.push_back({});
    }
    
    void EndRenderPassInRecordingStage()
    {
        // 在一个新的pass中记录绑定的bindGroup
        isInRenderPass_ = false;
    }
    
    virtual void
    BeginRenderPass(RenderPassPtr pass, FramebufferPtr framebuffer, QuerySetPtr occlusionQuerySet,
                    std::uint32_t clearValueCount, const Color* clearValues, float clearDepth,
                    uint32_t clearStencil) override;

    virtual void EndRenderPass() override;
    
    void ClearAttachment(VkClearAttachment clearAttachment);
    
    virtual void SetBindGroupToGraphicPipeline(std::uint32_t index, BindGroupPtr bindGroup,
                                               uint32_t dynamicOffsetCount = 0,
                                               const uint32_t* pDynamicOffsets = nullptr) override;
    
    virtual void SetBindGroupToComputePipeline(std::uint32_t index, BindGroupPtr bindGroup,
                                               uint32_t dynamicOffsetCount = 0,
                                               const uint32_t* pDynamicOffsets = nullptr) override;
    
    virtual void BindGraphicsPipeline(RenderPipelinePtr graphicPipeline) override;
    
    virtual void BindComputePipeline(ComputePipelinePtr computePipeline) override;
    
    virtual void SetIndexBuffer(BufferPtr buffer, std::uint32_t offset = 0) override;
    
    virtual void SetVertexBuffer(BufferPtr buffer, std::uint32_t offset = 0,
                                 std::uint32_t slot = 0) override;
    
    virtual void
    Dispatch(std::uint32_t groupCountX, std::uint32_t groupCountY,
             std::uint32_t groupCountZ) override;
    
    virtual void DispatchIndirect(BufferPtr indirectBuffer, BufferSize indirectOffset) override;
    
    void AddBindingBuffer(const BufferPtr& buffer);

    void AddBindingBindGroup(const BindGroupPtr& bindGroup);
    
    void ResetCommandBuffer();
    
    void BeginCommandBuffer();
    
    void EndCommandBuffer();
    
    virtual void Dispose() override;
    
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
    
    virtual void SetScissorRect(uint32_t x, uint32_t y, std::uint32_t width,
                                std::uint32_t height) override;
    
    virtual void SetStencilReference(std::uint32_t reference) override;
    
    virtual void SetBlendColor(const Color &color) override;

    virtual void SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override;
    
    virtual void PushDebugGroup(const char* groupLabel) override;
    
    virtual void PopDebugGroup() override;
    
    virtual void InsertDebugMarker(const char* markerLabel) override;
    
    virtual void BeginOcclusionQuery(std::uint32_t queryIndex) override;
    
    virtual void EndOcclusionQuery(std::uint32_t queryIndex) override;
    
    virtual void ResolveQuerySet(
        QuerySetPtr querySet,
        std::uint32_t queryFirstIndex,
        std::uint32_t queryCount,
        BufferPtr dstBuffer,
        std::uint32_t dstOffset) override;
    
    virtual void OnDeleteEvent() const override;
    
    void CheckViewportScissorBeforDraw();
    
    void MarkPresentSwapchain()
    { needPresentSwapchain_ = true; }
    
    bool NeedPresentSwapchain()
    { return needPresentSwapchain_; }
    
    VkDescriptorSet AsyncWriteBindGroupToGPU(VKBindGroup* bindGroup);
    
    bool BakeCmdBufferAsync(VkCommandBuffer vkCmdBuffer);
    
    void PostprocessCommandBuffer();

    static void SetupTexturePipelineBarrier(VKCommandBuffer* commandBuffer, const TexturePtr& texture_, TextureUsageFlags srcUsageFlags_, TextureUsageFlags dstUsageFlags_);

protected:
    void ForceEndRenderPass();
    
    //void SubmitCommandList();
    
    void SetIndexBufferInternal();

private:
    VkCommandBuffer vkCommandBuffer_ = VK_NULL_HANDLE;
    
   
    bool isRecording_ = false;
    bool hasSetViewport_ = false;
    bool hasSetScissor_ = false;
    bool needPresentSwapchain_ = false;
    bool bHasPendingEndRenderPass_ = false;
    bool isRenderPassPresentable_ = false;
    bool isInRenderPass_ = false;
    CommandListPtr pCommandList_ = nullptr;
    
    RenderPipelinePtr graphicPipelineBinding_ = nullptr;
    ComputePipelinePtr computePipelineBinding_ = nullptr;
    
    RenderPassPtr renderPassBinding_ = nullptr;
    FramebufferPtr framebufferBinding_ = nullptr;
    QuerySetPtr occlusionQuerySetBinding_ = nullptr;
    
    friend class VKDevice;
    
    struct BoundIndexBufferItem
    {
        BufferPtr indexBuffer = nullptr;
        std::uint32_t offset = 0;
    };
    
    BoundIndexBufferItem boundIndexBufferItem_;
    std::uint32_t currentRenderPassIndex_ = 0;

    // 将不同render pass中的bindGroup聚合在不同的列表中
    std::vector<std::vector<BindGroupPtr>> bindGroupCatagories_;
    std::vector<BufferPtr> bindBuffers_;
    // bool enableCalcBindBufferCount_ = true;
};

NS_GFX_END


#endif //RHI_VKCOMMANDBUFFER_H
