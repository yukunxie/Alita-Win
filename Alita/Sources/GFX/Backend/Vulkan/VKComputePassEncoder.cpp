//
// Created by realxie on 2020-01-19.
//

#include "VKComputePassEncoder.h"
#include "VKCommandBuffer.h"
#include "VKComputePipeline.h"
#include "VKBindGroup.h"

NS_GFX_BEGIN

VKComputePassEncoder::VKComputePassEncoder(const DevicePtr& device)
    : ComputePassEncoder(device)
{
}

VKComputePassEncoder::~VKComputePassEncoder()
{
    Dispose();
}

void VKComputePassEncoder::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    commandBuffer_ = nullptr;
    
    GFX_DISPOSE_END();
}

bool VKComputePassEncoder::Init(const CommandBufferPtr& commandBuffer, const ComputePassDescriptor &descriptor)
{
    commandBuffer_ = commandBuffer;
    return true;
}

void VKComputePassEncoder::SetPipeline(const ComputePipelinePtr& pipeline)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetComputePipeline>(pipeline);

    commandBuffer_->AddCmd([pipeline](CommandBuffer* commandBuffer) {
        commandBuffer->BindComputePipeline(pipeline);
        });
}

void VKComputePassEncoder::Dispatch(std::uint32_t x, std::uint32_t y, std::uint32_t z)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdDispatch>(x, y, z);

    commandBuffer_->AddCmd([x, y, z](CommandBuffer* commandBuffer) {
        commandBuffer->Dispatch(x, y, z);
        });

}

void VKComputePassEncoder::DispatchIndirect(const BufferPtr& indirectBuffer, BufferSize indirectOffset)
{
    //GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdDispatchIndirect>(indirectBuffer, indirectOffset);

    commandBuffer_->AddCmd([indirectBuffer, indirectOffset](CommandBuffer* commandBuffer) {
        commandBuffer->DispatchIndirect(indirectBuffer, indirectOffset);
        });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBuffer(indirectBuffer);
}

void VKComputePassEncoder::EndPass()
{
}

void
VKComputePassEncoder::SetBindGroup(std::uint32_t index, const BindGroupPtr& bindGroup, std::uint32_t count,
                                   std::uint32_t* dynamicOffsets)
{
    /*GFX_CAST(VKCommandBuffer*, commandBuffer_)->RecordCommand<DeferredCmdSetBindGroup>(PipelineType::Compute,
                                                           index,
                                                           bindGroup,
                                                           count,
                                                           dynamicOffsets);*/

    std::vector<std::uint32_t> tmpOffsets(count);
    if (count > 0)
    {
        memcpy(tmpOffsets.data(), dynamicOffsets, sizeof(std::uint32_t) * count);
    }

    commandBuffer_->AddCmd([index, bindGroup, dynamicOffsets = tmpOffsets](CommandBuffer* commandBuffer) {
        commandBuffer->SetBindGroupToComputePipeline(index,
            bindGroup,
            (std::uint32_t)dynamicOffsets.size(),
            dynamicOffsets.data());
    });

    GFX_CAST(VKCommandBuffer*, commandBuffer_)->AddBindingBindGroup(bindGroup);
}

NS_GFX_END