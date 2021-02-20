//
// Created by realxie on 2019-10-10.
//

#include "VKBindGroup.h"
#include "VKBuffer.h"
#include "VKTextureView.h"
#include "VKSampler.h"


NS_RHI_BEGIN

bool VKBindGroup::Init(VKDevice* device, const BindGroupDescriptor &descriptor)
{
    vkDevice_ = device->GetDevice();
    vkDescriptorPool_ = device->GetDescriptorPool();
    
    VkDescriptorSetLayout setLayout = RHI_CAST(const VKBindGroupLayout*,
                                               descriptor.layout)->GetNative();
    VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vkDescriptorPool_,
        .descriptorSetCount = 1,
        .pSetLayouts = &setLayout
    };
    
    CALL_VK(vkAllocateDescriptorSets(device->GetDevice(), &allocInfo, &vkDescriptorSet_));
    
    for (const auto res : descriptor.bindings)
    {
        bindingResources_.push_back(res);
    }
    return true;
}

VKBindGroup* VKBindGroup::Create(VKDevice* device, const BindGroupDescriptor &descriptor)
{
    auto ret = new VKBindGroup();
    if (ret && ret->Init(device, descriptor))
    {
        RHI_SAFE_RETAIN(ret);
        return ret;
    }
    
    if (ret) delete ret;
    return nullptr;
}


VKBindGroup::~VKBindGroup()
{
    if (vkDescriptorSet_)
    {
        vkFreeDescriptorSets(vkDevice_, vkDescriptorPool_, 1, &vkDescriptorSet_);
    }
}

void VKBindGroup::WriteToGPU() const
{
    for (const BindGroupBinding &binding : bindingResources_)
    {
        auto resource = binding.resource;
        switch (resource->GetResourceType())
        {
            case BindingResourceType::BUFFER:
            {
                auto bindingBuffer = RHI_CAST(const BufferBinding*, resource);
                auto* buffer = RHI_CAST(const VKBuffer*, bindingBuffer->buffer);
                VkDescriptorBufferInfo bufferInfo{
                    .buffer = buffer->GetNative(),
                    .offset = bindingBuffer->offset,
                    .range  = bindingBuffer->size,
                };
                
                VkWriteDescriptorSet descriptorWrite{
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = vkDescriptorSet_,
                    .dstBinding = binding.binding,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .pImageInfo = nullptr,
                    .pBufferInfo = &bufferInfo,
                    .pTexelBufferView = nullptr, // optional
                    
                };
                vkUpdateDescriptorSets(vkDevice_, 1, &descriptorWrite, 0, nullptr);
            }
                break;
            
            case BindingResourceType::SAMPLER:
            {
                VkDescriptorImageInfo imageInfo = {.sampler = RHI_CAST(const VKSampler*, resource)->GetNative()};
                
                VkWriteDescriptorSet descriptorWrite{
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = vkDescriptorSet_,
                    .dstBinding = binding.binding,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
                    .pImageInfo = &imageInfo,
                    .pBufferInfo = nullptr,
                    .pTexelBufferView = nullptr, // optional
                    
                };
                vkUpdateDescriptorSets(vkDevice_, 1, &descriptorWrite, 0, nullptr);
            }
                break;
            
            case BindingResourceType::TEXTURE_VIEW:
            {
                VkDescriptorImageInfo imageInfo = {
                    .imageView = RHI_CAST(const VKTextureView*, resource)->GetNative(),
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                };
    
                VkWriteDescriptorSet descriptorWrite{
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = vkDescriptorSet_,
                    .dstBinding = binding.binding,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                    .pImageInfo = &imageInfo,
                    .pBufferInfo = nullptr,
                    .pTexelBufferView = nullptr, // optional
        
                };
                vkUpdateDescriptorSets(vkDevice_, 1, &descriptorWrite, 0, nullptr);
            }
                break;
            
            case BindingResourceType::COMBINED_SAMPLER_TEXTUREVIEW:
            {
                auto combinedST = RHI_CAST(const CombinedSamplerImageViewBinding*, binding.resource);
                auto vkTextureView = RHI_CAST(const VKTextureView*, combinedST->imageView);
                auto vkSampler = RHI_CAST(const VKSampler*, combinedST->sampler);
                
                VkDescriptorImageInfo imageInfo = {
                    .sampler = vkSampler->GetNative(),
                    .imageView = vkTextureView->GetNative(),
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                };
    
                VkWriteDescriptorSet descriptorWrite{
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = vkDescriptorSet_,
                    .dstBinding = binding.binding,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = &imageInfo,
                    .pBufferInfo = nullptr,
                    .pTexelBufferView = nullptr, // optional
        
                };
                vkUpdateDescriptorSets(vkDevice_, 1, &descriptorWrite, 0, nullptr);
            }
                break;
        }
    }
}

void VKBindGroup::BindToCommandBuffer(std::uint32_t index, VkCommandBuffer vkCommandBuffer,
                                      VkPipelineLayout vkPipelineLayout) const
{
    vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout,
                            index, 1, &vkDescriptorSet_, 0, nullptr);
}

NS_RHI_END