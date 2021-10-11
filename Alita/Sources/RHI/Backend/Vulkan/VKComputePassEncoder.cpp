//
// Created by realxie on 2020-01-19.
//

#include "VKComputePassEncoder.h"
#include "VKCommandBuffer.h"
#include "VKComputePipeline.h"
#include "VKBindGroup.h"

NS_GFX_BEGIN

VKComputePassEncoder::VKComputePassEncoder(VKDevice* device)
    : ComputePassEncoder(device)
{
}

VKComputePassEncoder::~VKComputePassEncoder()
{
    Dispose();
}

void VKComputePassEncoder::Dispose()
{
    RHI_DISPOSE_BEGIN();
    
    RHI_SAFE_RELEASE(commandBuffer_);
    
    RHI_DISPOSE_END();
}

bool VKComputePassEncoder::Init(VKCommandBuffer* commandBuffer, const ComputePassDescriptor &descriptor)
{
    RHI_PTR_ASSIGN(commandBuffer_, commandBuffer);
    return true;
}

void VKComputePassEncoder::SetPipeline(ComputePipeline* pipeline)
{
    commandBuffer_->RecordCommand<DeferredCmdSetComputePipeline>(pipeline);
    commandBuffer_->AddBindingObject(pipeline);
}

void VKComputePassEncoder::Dispatch(std::uint32_t x, std::uint32_t y, std::uint32_t z)
{
    commandBuffer_->RecordCommand<DeferredCmdDispatch>(x, y, z);
}

void VKComputePassEncoder::DispatchIndirect(Buffer* indirectBuffer, BufferSize indirectOffset)
{
    commandBuffer_->RecordCommand<DeferredCmdDispatchIndirect>(indirectBuffer, indirectOffset);
    commandBuffer_->AddBindingObject(indirectBuffer);
}

void VKComputePassEncoder::EndPass()
{
}

void
VKComputePassEncoder::SetBindGroup(std::uint32_t index, BindGroup* bindGroup, std::uint32_t count,
                                   std::uint32_t* dynamicOffsets)
{
    commandBuffer_->RecordCommand<DeferredCmdSetBindGroup>(PipelineType::Compute,
                                                           index,
                                                           bindGroup,
                                                           count,
                                                           dynamicOffsets);
    commandBuffer_->AddBindingObject(bindGroup);
}

NS_GFX_END