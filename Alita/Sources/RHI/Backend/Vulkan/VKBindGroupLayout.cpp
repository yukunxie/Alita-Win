//
// Created by realxie on 2019-10-10.
//

#include "VKBindGroupLayout.h"
#include "VKTypes.h"

NS_RHI_BEGIN

VKBindGroupLayout::VKBindGroupLayout(VKDevice* device)
    : BindGroupLayout(device)
{
}

bool VKBindGroupLayout::Init(const BindGroupLayoutDescriptor &descriptor)
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    for (size_t i = 0; i < descriptor.entries.size(); ++i)
    {
        const auto &binding = descriptor.entries[i];
        
        VkDescriptorSetLayoutBinding layoutBinding{
            .binding = binding.binding,
            .descriptorType = ToVulkanType(binding.type),
            .descriptorCount= 1,
            .stageFlags = GetVkShaderStageFlags(binding.visibility),
            .pImmutableSamplers = nullptr,
        };
        layoutBindings.push_back(layoutBinding);
        vkDescriptorTypes_.push_back({binding.binding, layoutBinding.descriptorType});
        
        /**
         * https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCmdBindDescriptorSets.html
         * If any of the sets being bound include dynamic uniform or storage buffers, then pDynamicOffsets includes one element for each array
         * element in each dynamic descriptor type binding in each set. Values are taken from pDynamicOffsets in an order such that all
         * entries for set N come before set N+1; within a set, entries are ordered by the binding numbers in the descriptor set layouts; and
         * within a binding array, elements are in order. dynamicOffsetCount must equal the total number of dynamic descriptors in the sets
         * being bound.
         */
        if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
            layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
            layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
        {
            dynamicOffsetCount_++;
        }
    }
    
    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .bindingCount = (std::uint32_t) layoutBindings.size(),
        .pBindings = layoutBindings.data(),
    };
    
    CALL_VK(vkCreateDescriptorSetLayout(VKDEVICE()->GetNative(), &layoutInfo, nullptr,
                                          &vkBindGroupLayout_));
    
    return VK_NULL_HANDLE != vkBindGroupLayout_;
}

void VKBindGroupLayout::Dispose()
{
    RHI_DISPOSE_BEGIN();
    
    if (vkBindGroupLayout_)
    {
        VkDevice deviceVk = RHI_CAST(VKDevice*, GetGPUDevice())->GetNative();
        vkDestroyDescriptorSetLayout(deviceVk, vkBindGroupLayout_, nullptr);
        vkBindGroupLayout_ = VK_NULL_HANDLE;
    }
    
    RHI_DISPOSE_END();
}

VKBindGroupLayout::~VKBindGroupLayout()
{
    Dispose();
}

NS_RHI_END
