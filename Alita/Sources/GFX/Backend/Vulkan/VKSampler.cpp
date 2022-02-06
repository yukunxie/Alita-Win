//
// Created by realxie on 2019-10-07.
//

#include "VKSampler.h"

NS_GFX_BEGIN

VKSampler::VKSampler(const DevicePtr& device)
    : Sampler(device)
{
}

void VKSampler::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    if (vkSampler_)
    {
        vkDestroySampler(VKDEVICE()->GetNative(), vkSampler_, nullptr);
        vkSampler_ = VK_NULL_HANDLE;
    }
    
    GFX_DISPOSE_END();
}

VKSampler::~VKSampler()
{
    Dispose();
}

bool VKSampler::Init(const SamplerDescriptor &descriptor)
{
    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = ToVulkanType(descriptor.magFilter),
        .minFilter = ToVulkanType(descriptor.minFilter),
        .mipmapMode = GetVkSamplerMipmapMode(descriptor.minFilter),
        .addressModeU = ToVulkanType(descriptor.addressModeU),
        .addressModeV = ToVulkanType(descriptor.addressModeV),
        .addressModeW = ToVulkanType(descriptor.addressModeW),
        .mipLodBias = 0.0f,
        .maxAnisotropy = 1,
        .compareOp = ToVulkanType(descriptor.compare),
        .minLod = descriptor.lodMinClamp,
        .maxLod = descriptor.lodMaxClamp,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
        .unnormalizedCoordinates = VK_FALSE,
    };
    
    CALL_VK(vkCreateSampler(VKDEVICE()->GetNative(), &samplerInfo, nullptr, &vkSampler_));
    
    return VK_NULL_HANDLE != vkSampler_;
}

NS_GFX_END