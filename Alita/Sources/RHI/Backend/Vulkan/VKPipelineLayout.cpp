//
// Created by realxie on 2019-10-10.
//

#include "VKPipelineLayout.h"

NS_RHI_BEGIN

bool VKPipelineLayout::Init(VKDevice* device, const PipelineLayoutDescriptor &descriptor)
{
    vkDevice_ = device->GetDevice();
    
    std::vector<VkDescriptorSetLayout> setLayouts;
    for (const BindGroupLayout* bindGroupLayout: descriptor.bindGroupLayouts)
    {
        setLayouts.push_back(((VKBindGroupLayout*) bindGroupLayout)->GetNative());
    }
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = (std::uint32_t) setLayouts.size(),
        .pSetLayouts = setLayouts.data(),
        .pushConstantRangeCount = 0, // Optional
        .pPushConstantRanges = nullptr, // Optional
    };
    
    CALL_VK(vkCreatePipelineLayout(vkDevice_, &pipelineLayoutInfo, nullptr, &vkPipelineLayout_));
    
    return true;
}

VKPipelineLayout*
VKPipelineLayout::Create(VKDevice* device, const PipelineLayoutDescriptor &descriptor)
{
    auto ret = new VKPipelineLayout();
    if (ret && ret->Init(device, descriptor))
    {
        RHI_SAFE_RETAIN(ret);
        return ret;
    }
    
    if (ret) delete ret;
    return nullptr;
}

VKPipelineLayout::~VKPipelineLayout()
{
    if (vkPipelineLayout_)
    {
        vkDestroyPipelineLayout(vkDevice_, vkPipelineLayout_, nullptr);
    }
}

NS_RHI_END