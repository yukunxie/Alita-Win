//
// Created by realxie on 2019-10-14.
//

#include "VKCommandBuffer.h"

NS_RHI_BEGIN

VKCommandBuffer::VKCommandBuffer(VKDevice* device)
    : device_(device)
{
    VkCommandBufferAllocateInfo cmdBufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = device->GetCommandBufferPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    
    CALL_VK(vkAllocateCommandBuffers(device->GetDevice(), &cmdBufferCreateInfo, &vkCommandBuffer_));
}

VKCommandBuffer::~VKCommandBuffer()
{
    vkFreeCommandBuffers(device_->GetDevice(), device_->GetCommandBufferPool(), 1,
                         &vkCommandBuffer_);
}

void VKCommandBuffer::ResetCommandBuffer()
{
}


NS_RHI_END