//
// Created by realxie on 2020/8/27.
//

#ifndef PROJ_ANDROID_RENDERTHREADINGVKALLOCATOR_H
#define PROJ_ANDROID_RENDERTHREADINGVKALLOCATOR_H

#include "VKDevice.h"

NS_GFX_BEGIN

class ThreadLocalVulkanAllocator
{
public:
    ThreadLocalVulkanAllocator(VkDevice vkDevice)
        : vkDevice_(vkDevice)
    {
        VkCommandPoolCreateInfo poolInfo = {};
        {
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.pNext = nullptr;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = 0;
        };
        CALL_VK(vkCreateCommandPool(vkDevice_, &poolInfo, nullptr, &vkCommandPool_));
    }
    
    ~ThreadLocalVulkanAllocator()
    {
        if (vkCommandPool_)
        {
            vkFreeCommandBuffers(vkDevice_, vkCommandPool_, vkCommandBuffers_.size(),
                                   vkCommandBuffers_.data());
            vkCommandBuffers_.clear();
            
            vkDestroyCommandPool(vkDevice_, vkCommandPool_, nullptr);
            vkCommandPool_ = VK_NULL_HANDLE;
        }
    }
    
    VkCommandBuffer AcquireVkCommandBuffer()
    {
        if (vkCommandBuffers_.empty())
        {
            std::array<VkCommandBuffer, 3> commandBuffers;
            
            VkCommandBufferAllocateInfo cmdBufferCreateInfo;
            {
                cmdBufferCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                cmdBufferCreateInfo.pNext = nullptr;
                cmdBufferCreateInfo.commandPool = vkCommandPool_;
                cmdBufferCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                cmdBufferCreateInfo.commandBufferCount = commandBuffers.size();
            };
            CALL_VK(vkAllocateCommandBuffers(vkDevice_,
                                               &cmdBufferCreateInfo,
                                               commandBuffers.data()));
            
            for (size_t i = 0; i < commandBuffers.size(); ++i)
            {
                vkCommandBuffers_.push_back(commandBuffers[i]);
            }
        }
        
        auto vkCommandBuffer = vkCommandBuffers_.back();
        vkCommandBuffers_.pop_back();
        return vkCommandBuffer;
    }
    
    void ReleaseVkCommandBuffer(VkCommandBuffer vkCommandBuffer)
    {
        if (vkCommandBuffer)
        {
            vkResetCommandBuffer(vkCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            vkCommandBuffers_.push_back(vkCommandBuffer);
        }
    }

private:
    VkDevice vkDevice_ = VK_NULL_HANDLE;
    VkCommandPool vkCommandPool_ = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> vkCommandBuffers_;
};

NS_GFX_END

#endif //PROJ_ANDROID_RENDERTHREADINGVKALLOCATOR_H
