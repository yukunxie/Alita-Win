//
// Created by realxie on 2019-10-02.
//

#include "VKBuffer.h"
#include "VKTypes.h"

NS_RHI_BEGIN

VKBuffer::VKBuffer(VKDevice* device, const BufferDescriptor &descriptor)
{
    vkDevice_ = device->GetDevice();
    
    uint32_t queueFamilyIndex_ = device->GetQueueFamilyIndices().graphicsFamily;
    
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = descriptor.size,
        .usage = GetVkBufferUsageFlags(descriptor.usage),
        .sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &queueFamilyIndex_,
    };
    
    CALL_VK(vkCreateBuffer(vkDevice_, &bufferInfo, nullptr, &vkBuffer_));
    
    // Allocate buffer memory
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkDevice_, vkBuffer_, &memRequirements);
    
    std::uint32_t memTypeMask =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits, memTypeMask)
    };
    
    CALL_VK(vkAllocateMemory(vkDevice_, &allocInfo, nullptr, &vkBufferMemory_));
    CALL_VK(vkBindBufferMemory(vkDevice_, vkBuffer_, vkBufferMemory_, 0));
}

VKBuffer::~VKBuffer()
{
    // TODO release vulkan resource.
}

void VKBuffer::UpdateBuffer(const void* data, std::uint32_t offset, std::uint32_t size)
{
    void* accessPointer;
    CALL_VK(vkMapMemory(vkDevice_, vkBufferMemory_, offset, size, 0, &accessPointer));
    memcpy(accessPointer, data, (size_t) size);
    vkUnmapMemory(vkDevice_, vkBufferMemory_);
}

const void* VKBuffer::MapReadAsync(std::uint32_t offset, std::uint32_t size)
{
    RHI_ASSERT(pData_ == nullptr);
    /**
     * flag must be zero now
     * VkMemoryMapFlags is a bitmask type for setting a mask, but is currently reserved for future use.
     */
    VkMemoryMapFlags flag = 0;
    CALL_VK(vkMapMemory(vkDevice_, vkBufferMemory_, offset, size > 0 ? size : VK_WHOLE_SIZE, flag,
                        &pData_));
    return pData_;
}

void* VKBuffer::MapWriteAsync(std::uint32_t offset, std::uint32_t size)
{
    RHI_ASSERT(pData_ == nullptr);
    /**
     * flag must be zero now
     * VkMemoryMapFlags is a bitmask type for setting a mask, but is currently reserved for future use.
     */
    VkMemoryMapFlags flag = 0;
    CALL_VK(vkMapMemory(vkDevice_, vkBufferMemory_, offset, size > 0 ? size : VK_WHOLE_SIZE, flag,
                        &pData_));
    return pData_;
}

const void* VKBuffer::MapReadAsync()
{
    return MapReadAsync(0, 0);
}

void* VKBuffer::MapWriteAsync()
{
    return MapWriteAsync(0, 0);
}

void VKBuffer::Unmap()
{
    RHI_ASSERT(pData_);
    vkUnmapMemory(vkDevice_, vkBufferMemory_);
    pData_ = nullptr;
}

void VKBuffer::Destroy()
{
}

void VKBuffer::SetSubData(std::uint32_t offset, std::uint32_t byteSize, const void* data)
{
    RHI_ASSERT(byteSize > 0);
    auto pAddress = (std::uint8_t*) MapWriteAsync(offset, byteSize);
    RHI_ASSERT(pAddress);
    memcpy(pAddress, data, byteSize);
    Unmap();
}

NS_RHI_END
