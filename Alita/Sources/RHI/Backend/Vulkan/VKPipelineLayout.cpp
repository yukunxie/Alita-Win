//
// Created by realxie on 2019-10-10.
//

#include "VKPipelineLayout.h"

NS_GFX_BEGIN

VKPipelineLayout::VKPipelineLayout(VKDevice* device)
    : PipelineLayout(device)
{

}

void VKPipelineLayout::Dispose()
{
    RHI_DISPOSE_BEGIN();
    
    if (vkPipelineLayout_)
    {
        vkDestroyPipelineLayout(VKDEVICE()->GetNative(), vkPipelineLayout_, nullptr);
        vkPipelineLayout_ = VK_NULL_HANDLE;
    }
    
    RHI_DISPOSE_END();
}

VKPipelineLayout::~VKPipelineLayout()
{
    Dispose();
}

bool VKPipelineLayout::Init(const PipelineLayoutDescriptor &descriptor)
{
    std::vector<VkDescriptorSetLayout> setLayouts;
    for (size_t i = 0; i < descriptor.bindGroupLayouts.size(); ++i)
    {
        const BindGroupLayout* bindGroupLayout = descriptor.bindGroupLayouts[i];
        setLayouts.push_back(((VKBindGroupLayout*) bindGroupLayout)->GetNative());
    }
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = (std::uint32_t) setLayouts.size(),
        .pSetLayouts = setLayouts.data(),
        .pushConstantRangeCount = 0, // Optional
        .pPushConstantRanges = nullptr, // Optional
    };
    
    CALL_VK(vkCreatePipelineLayout(VKDEVICE()->GetNative(), &pipelineLayoutInfo, nullptr,
                                     &vkPipelineLayout_));
    return VK_NULL_HANDLE != vkPipelineLayout_;
}

NS_GFX_END