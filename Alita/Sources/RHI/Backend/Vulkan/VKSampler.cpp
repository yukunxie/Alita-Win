//
// Created by realxie on 2019-10-07.
//

#include "VKSampler.h"

NS_RHI_BEGIN

bool VKSampler::Init(VKDevice* device, const SamplerDescriptor &descriptor)
{
    vkDevice_ = device->GetDevice();
    
    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = GetVkFilter(descriptor.magFilter),
        .minFilter = GetVkFilter(descriptor.minFilter),
        .mipmapMode = GetVkSamplerMipmapMode(descriptor.minFilter),
        .addressModeU = GetVkSamplerAddressMode(descriptor.addressModeU),
        .addressModeV = GetVkSamplerAddressMode(descriptor.addressModeV),
        .addressModeW = GetVkSamplerAddressMode(descriptor.addressModeW),
        .mipLodBias = 0.0f,
        .maxAnisotropy = 1,
        .compareOp = GetCompareOp(descriptor.compare),
        .minLod = descriptor.lodMinClamp,
        .maxLod = descriptor.lodMaxClamp,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
        .unnormalizedCoordinates = VK_FALSE,
    };
    
    CALL_VK(vkCreateSampler(vkDevice_, &samplerInfo, nullptr, &vkSampler_));
    
    return true;
}

VKSampler::~VKSampler()
{
    vkDestroySampler(vkDevice_, vkSampler_, nullptr);
}

NS_RHI_END