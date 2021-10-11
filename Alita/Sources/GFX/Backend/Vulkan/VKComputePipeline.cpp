//
// Created by realxie on 2020-01-19.
//

#include "VKComputePipeline.h"
#include "VKShader.h"
#include "VKPipelineLayout.h"
#include "VKRenderPass.h"

NS_GFX_BEGIN

VKComputePipeline::VKComputePipeline(VKDevice* device)
    : ComputePipeline(device)
{
}

VKComputePipeline::~VKComputePipeline()
{
    Dispose();
}

void VKComputePipeline::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    if (VK_NULL_HANDLE != vkComputePipeline_)
    {
        vkDestroyPipeline(VKDEVICE()->GetNative(), vkComputePipeline_, VK_NULL_HANDLE);
        vkComputePipeline_ = VK_NULL_HANDLE;
    }
    
    GFX_SAFE_RELEASE(pipelineLayout_);
    GFX_SAFE_RELEASE(renderPass_);
    
    GFX_DISPOSE_END();
}

bool VKComputePipeline::Init(const ComputePipelineDescriptor &descriptor)
{
    GFX_PTR_ASSIGN(pipelineLayout_, GFX_CAST(VKPipelineLayout * , descriptor.layout));
    
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;
    
    VkComputePipelineCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags =
        VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | VK_PIPELINE_CREATE_DERIVATIVE_BIT |
        VK_PIPELINE_CREATE_DISPATCH_BASE;
    createInfo.layout = pipelineLayout_->GetNative();
    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = -1;
    
    createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage.pNext = nullptr;
    createInfo.stage.flags = 0;
    createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    createInfo.stage.module = GFX_CAST(VKShader*, descriptor.computeStage.shader)->GetNative();
    createInfo.stage.pName = descriptor.computeStage.entryPoint.c_str();
    createInfo.stage.pSpecializationInfo = nullptr;
    
    CALL_VK(
        vkCreateComputePipelines(VKDEVICE()->GetNative(), pipelineCache, 1, &createInfo, nullptr,
                                   &vkComputePipeline_));
    
    return VK_NULL_HANDLE != vkComputePipeline_;
}

VkPipelineLayout VKComputePipeline::GetPipelineLayout() const
{
    return pipelineLayout_->GetNative();
}

NS_GFX_END