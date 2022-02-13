//
// Created by realxie on 2019-10-10.
//

#include "VKBindGroup.h"
#include "VKBuffer.h"
#include "VKTextureView.h"
#include "VKSampler.h"
#include "VKCommandBuffer.h"
#include "VKRenderPipeline.h"
#include "RenderThreading.h"


NS_GFX_BEGIN

VKBindGroup::VKBindGroup(const DevicePtr& device)
    : BindGroup(device)
{
}

bool VKBindGroup::Init(BindGroupDescriptor &descriptor)
{
    bindGroupLayout_ = descriptor.layout;
    
    bindingResources_.resize(descriptor.entries.size());
    for (size_t i = 0; i < descriptor.entries.size(); ++i)
    {
        auto &res = descriptor.entries[i];
        bindingResources_[i] = res;
    }
    
    VkDescriptorSetLayout layout = GFX_CAST(VKBindGroupLayout*, bindGroupLayout_)->GetNative();
    VkDescriptorSetAllocateInfo allocInfo;
    {
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;
        allocInfo.descriptorPool = VKDEVICE()->GetDescriptorPool();
    }
    
    CALL_VK(
        vkAllocateDescriptorSets(VKDEVICE()->GetNative(), &allocInfo,
                                   &vkDescriptorSet_));
    
    return vkDescriptorSet_ != VK_NULL_HANDLE;
}

void VKBindGroup::UpdateDescriptorSetAsync()
{
    if (vkDescriptorSet_ == VK_NULL_HANDLE || hasUpdateDescriptorSet_)
    {
        return;
    }
    
    hasUpdateDescriptorSet_ = true;
    VkDevice deviceVk = GFX_CAST(VKDevice*, GetGPUDevice())->GetNative();
    
    for (std::uint32_t i = 0; i < bindingResources_.size(); ++i)
    {
        const auto &binding = bindingResources_[i];
        auto resource = binding.resource;
        
        switch (resource->GetResourceType())
        {
            case BindingResourceType::Buffer:
            {
                auto bindingBuffer = GFX_CAST(BufferBinding*, resource);
                auto* buffer = GFX_CAST(VKBuffer*, bindingBuffer->GetBuffer());
                VkDescriptorBufferInfo bufferInfo;
                {
                    bufferInfo.buffer = buffer->GetNative();
                    bufferInfo.offset = bindingBuffer->GetOffset();
                    bufferInfo.range = bindingBuffer->GetSize();
                };
                
                if (bindingBuffer->GetSize() == -1)
                {
                    bufferInfo.range = VK_WHOLE_SIZE;
                }
                
                VkWriteDescriptorSet descriptorWrite;
                {
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.pNext = nullptr;
                    descriptorWrite.dstSet = vkDescriptorSet_;
                    descriptorWrite.dstBinding = binding.binding;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorType = GFX_CAST(VKBindGroupLayout*, bindGroupLayout_)->GetDescriptorType(
                        binding.binding);
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pBufferInfo = &bufferInfo;
                    descriptorWrite.pImageInfo = nullptr;
                    descriptorWrite.pTexelBufferView = nullptr; // optional
                    
                };
                vkUpdateDescriptorSets(deviceVk, 1,
                                         &descriptorWrite, 0, nullptr);
            }
                break;
            
            case BindingResourceType::Sampler:
            {
                VkDescriptorImageInfo imageInfo;
                SamplerBinding* samplerBinding = GFX_CAST(SamplerBinding*, resource);
                imageInfo.sampler = GFX_CAST(VKSampler*, samplerBinding->GetSampler())->GetNative();
                
                VkWriteDescriptorSet descriptorWrite;
                {
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.pNext = nullptr;
                    descriptorWrite.dstSet = vkDescriptorSet_;
                    descriptorWrite.dstBinding = binding.binding;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pBufferInfo = nullptr;
                    descriptorWrite.pImageInfo = &imageInfo;
                    descriptorWrite.pTexelBufferView = nullptr; // optional
                }
                vkUpdateDescriptorSets(deviceVk, 1,
                                         &descriptorWrite, 0, nullptr);
            }
                break;
            
            case BindingResourceType::TextureView:
            {
                VkDescriptorImageInfo imageInfo;
                {
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    TextureViewBinding* textureViewBinding = GFX_CAST(TextureViewBinding*,
                                                                      resource);
                    VKTextureView* textureView = GFX_CAST(VKTextureView*,
                                                          textureViewBinding->GetTextureView());
                    imageInfo.imageView = textureView->GetNative();
                }
                
                VkWriteDescriptorSet descriptorWrite;
                {
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.pNext = nullptr;
                    descriptorWrite.dstSet = vkDescriptorSet_;
                    descriptorWrite.dstBinding = binding.binding;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pBufferInfo = nullptr;
                    descriptorWrite.pImageInfo = &imageInfo;
                    descriptorWrite.pTexelBufferView = nullptr; // optional
                }
                vkUpdateDescriptorSets(deviceVk, 1,
                                         &descriptorWrite, 0, nullptr);
            }
                break;
                
                // case RHIObjectType::SampledTextureView:
                // {
                //     auto combinedST = GFX_CAST(SampledTextureView*, binding.resource);
                //     auto vkTextureView = GFX_CAST(VKTextureView*, combinedST->GetTextureView());
                //     auto vkSampler = GFX_CAST(VKSampler *, combinedST->GetSampler());
                //
                //     VkDescriptorImageInfo imageInfo;
                //     {
                //         imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                //         imageInfo.imageView = vkTextureView->GetNative();
                //         imageInfo.sampler = vkSampler->GetNative();
                //     };
                //
                //     VkWriteDescriptorSet descriptorWrite;
                //     {
                //         descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                //         descriptorWrite.dstSet = vkDescriptorSet_;
                //         descriptorWrite.dstBinding = binding.binding;
                //         descriptorWrite.dstArrayElement = 0;
                //         descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                //         descriptorWrite.descriptorCount = 1;
                //         descriptorWrite.pBufferInfo = nullptr;
                //         descriptorWrite.pImageInfo = &imageInfo;
                //         descriptorWrite.pTexelBufferView = nullptr; // optional
                //     }
                //     vkUpdateDescriptorSets(GFX_CAST(VKDevice*, GetGPUDevice())->GetNative(), 1, &descriptorWrite, 0, nullptr);
                // }
                //     break;
            
            default:
                GFX_ASSERT(false);
                break;
        }
    }
}

void VKBindGroup::TransImageLayoutToSampled(VKCommandBuffer *commandBuffer)
{
    for (size_t i = 0; i < bindingResources_.size(); ++i)
    {
        auto resource = bindingResources_[i].resource;
        if (BindingResourceType::TextureView == resource->GetResourceType())
        {
            TextureViewBinding* textureViewBinding = GFX_CAST(TextureViewBinding*,
                                                              resource);
            VKTextureView* textureView = GFX_CAST(VKTextureView*,
                                                  textureViewBinding->GetTextureView());
            
            GFX_CAST(VKTexture*, textureView->GetTexture())->TransToSampledImageLayout(commandBuffer);
        }
    }
}

VKBindGroupLayout* VKBindGroup::GetBindGroupLayout()
{
    return GFX_CAST(VKBindGroupLayout*, bindGroupLayout_);
}

void VKBindGroup::Dispose()
{
    GFX_DISPOSE_BEGIN();

    bindingResources_.clear();

    CALL_VK(vkFreeDescriptorSets(VKDEVICE()->GetNative(),
        VKDEVICE()->GetDescriptorPool(),
        1,
        &vkDescriptorSet_));

    vkDescriptorSet_ = VK_NULL_HANDLE;

    bindGroupLayout_.reset();

    GFX_DISPOSE_END();
}

VKBindGroup::~VKBindGroup()
{
    Dispose();
}


std::vector<BufferPtr> VKBindGroup::getBindingBuffers()
{
    std::vector<BufferPtr> result;

    for (std::uint32_t i = 0; i < bindingResources_.size(); ++i)
    {
        const auto &binding = bindingResources_[i];
        auto resource = binding.resource;
        
        if (resource->GetResourceType() != BindingResourceType::Buffer) 
            continue;

        auto bindingBuffer = GFX_CAST(BufferBinding*, resource);
        result.push_back(bindingBuffer->GetBuffer());
    }
    return std::move(result);
}

NS_GFX_END