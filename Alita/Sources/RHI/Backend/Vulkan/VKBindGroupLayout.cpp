//
// Created by realxie on 2019-10-10.
//

#include "VKBindGroupLayout.h"
#include "VKTypes.h"

NS_RHI_BEGIN

bool VKBindGroupLayout::Init(VKDevice* device, const BindGroupLayoutDescriptor &descriptor)
{
    vkDevice_ = device->GetDevice();
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    for (const auto &binding : descriptor.bindings)
    {
        VkDescriptorSetLayoutBinding layoutBinding{
            .binding = binding.binding,
            .descriptorType = GetVkDescriptorType(binding.type),
            .descriptorCount= 1,
            .stageFlags = GetVkShaderStageFlags(binding.visibility),
            .pImmutableSamplers = nullptr,
        };
        layoutBindings.push_back(layoutBinding);
    }
    
    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .bindingCount = (std::uint32_t) layoutBindings.size(),
        .pBindings = layoutBindings.data(),
    };
    
    CALL_VK(vkCreateDescriptorSetLayout(vkDevice_, &layoutInfo, nullptr, &vkBindGroupLayout_));
    
    return true;
}

VKBindGroupLayout*
VKBindGroupLayout::Create(VKDevice* device, const BindGroupLayoutDescriptor &descriptor)
{
    auto ret = new VKBindGroupLayout();
    if (ret && ret->Init(device, descriptor))
    {
        RHI_SAFE_RETAIN(ret);
        return ret;
    }
    if (ret) delete ret;
    return nullptr;
}

VKBindGroupLayout::~VKBindGroupLayout()
{
    vkDestroyDescriptorSetLayout(vkDevice_, vkBindGroupLayout_, nullptr);
}

NS_RHI_END
