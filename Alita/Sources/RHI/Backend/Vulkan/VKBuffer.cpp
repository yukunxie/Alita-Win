//
// Created by realxie on 2019-10-02.
//

#include "VKBuffer.h"
#include "VKTypes.h"
#include "RenderThreading.h"

NS_GFX_BEGIN

VKBuffer::VKBuffer(VKDevice* device)
    : Buffer(device)
{
}

void VKBuffer::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    if (pData_)
    {
        Unmap();
    }
    
    onMapReadReady_ = nullptr;
    onMapWriteReady_ = nullptr;

#if USE_VULKAN_MEMORY_ALLCATOR
    vmaDestroyBuffer(VKDEVICE()->GetVmaAllocator(), vkBuffer_, vmaAllocation_);
    vmaAllocation_ = nullptr;
#else
    VkDevice deviceVk = VKDEVICE()->GetNative();

    if (vkBufferMemory_)
    {
        vkFreeMemory(deviceVk, vkBufferMemory_, nullptr);
        vkBufferMemory_ = VK_NULL_HANDLE;
    }

    if (vkBuffer_)
    {
        vkDestroyBuffer(deviceVk, vkBuffer_, nullptr);
        vkBuffer_ = VK_NULL_HANDLE;
    }
#endif
    
    GFX_DISPOSE_END();
}

VKBuffer::~VKBuffer()
{
    Dispose();
}

bool VKBuffer::Init(const BufferDescriptor &descriptor)
{
    Buffer::Init(descriptor);
    
    auto device = VKDEVICE();
    
    uint32_t queueFamilyIndex_ = device->GetQueueFamilyIndices().graphicsFamily;
    
    VkBufferCreateInfo bufferInfo;
    {
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.flags = 0;
        bufferInfo.size = bufferSize_;
        bufferInfo.usage = GetVkBufferUsageFlags(descriptor.usage);
        bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.pQueueFamilyIndices = &queueFamilyIndex_;
        bufferInfo.queueFamilyIndexCount = 1;
    }

#if USE_VULKAN_MEMORY_ALLCATOR
    VmaAllocationCreateInfo vmaAllocCreateInfo = {};
    vmaAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    vmaAllocCreateInfo.flags = 0;
    vmaAllocCreateInfo.preferredFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    
    vmaCreateBuffer(VKDEVICE()->GetVmaAllocator(), &bufferInfo, &vmaAllocCreateInfo, &vkBuffer_,
                    &vmaAllocation_, &vmaAllocationInfo_);
    
    return VK_NULL_HANDLE != vkBuffer_;
#else
    VkDevice deviceVk = VKDEVICE()->GetNative();
    
    CALL_VK(vkCreateBuffer(deviceVk, &bufferInfo, nullptr, &vkBuffer_));
    
    GFX_ASSERT(vkBuffer_ != VK_NULL_HANDLE);
    
    //
    // // Allocate buffer memory
    //
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(deviceVk, vkBuffer_, &memRequirements);
    //
    std::uint32_t memTypeMask =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    
    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits, memTypeMask)
    };
    
    CALL_VK(vkAllocateMemory(deviceVk, &allocInfo, nullptr, &vkBufferMemory_));
    CALL_VK(vkBindBufferMemory(deviceVk, vkBuffer_, vkBufferMemory_, 0));
    
    return VK_NULL_HANDLE != vkBuffer_ && VK_NULL_HANDLE != vkBufferMemory_;
#endif
}

void VKBuffer::UpdateBuffer(const void* data, std::uint32_t offset, std::uint32_t size)
{
    void* accessPointer = MapWrite(offset, size);
    memcpy(accessPointer, data, (size_t) size);
    Unmap();
}

const void* VKBuffer::MapRead(std::uint32_t offset, std::uint32_t size) const
{
    GFX_ASSERT(pData_ == nullptr);
    /**
     * flag must be zero now
     * VkMemoryMapFlags is a bitmask type for setting a mask, but is currently reserved for future use.
     */
#if USE_VULKAN_MEMORY_ALLCATOR
    vmaMapMemory(VKDEVICE()->GetVmaAllocator(), vmaAllocation_, &pData_);
    pData_ = (std::uint8_t*) pData_ + offset;
#else
    VkMemoryMapFlags flag = 0;
    CALL_VK(
        vkMapMemory(VKDEVICE()->GetNative(), vkBufferMemory_, offset,
                      size > 0 ? size : VK_WHOLE_SIZE,
                      flag,
                      &pData_));
#endif
    return pData_;
}

void* VKBuffer::MapWrite(std::uint32_t offset, std::uint32_t size) const
{
    GFX_ASSERT(pData_ == nullptr);

#if USE_VULKAN_MEMORY_ALLCATOR
    vmaMapMemory(VKDEVICE()->GetVmaAllocator(), vmaAllocation_, &pData_);
    pData_ = (std::uint8_t*) pData_ + offset;
#else
    /**
     * flag must be zero now
     * VkMemoryMapFlags is a bitmask type for setting a mask, but is currently reserved for future use.
     */
    VkMemoryMapFlags flag = 0;
    CALL_VK(
        vkMapMemory(VKDEVICE()->GetNative(), vkBufferMemory_, offset,
                      size > 0 ? size : VK_WHOLE_SIZE,
                      flag,
                      &pData_));
#endif
    return pData_;
}

void VKBuffer::MapReadAsync(
    const std::function<void(bool, Buffer*, const void*)> &onMapReadReady, 
    std::uint32_t offset, std::uint32_t size)
{
    if (onMapReadReady == nullptr) {
        onMapReadReady_ = nullptr;
        return;
    }

    if (getBindCount() > 0) {
        onMapReadReady_ = onMapReadReady;
        VKDevice* device = VKDEVICE();
        device->ScheduleAsyncTask<AsyncTaskBufferMapRead>(device, this, offset, size);
    }
    else {
        auto data = MapRead(0, GetBufferSize());
        onMapReadReady(true, this, data);
    }
}

void VKBuffer::MapWriteAsync(const std::function<void(bool, Buffer*, void*)> &onMapWriteReady, 
    std::uint32_t offset, std::uint32_t size)
{
    if (onMapWriteReady == nullptr) {
        onMapWriteReady_ = nullptr;
        return;
    }

    if (getBindCount() > 0) {
        onMapWriteReady_ = onMapWriteReady;
        VKDevice* device = VKDEVICE();
        device->ScheduleAsyncTask<AsyncTaskBufferMapWrite>(device, this, offset, size);
    }
    else {
        auto data = MapWrite(0, GetBufferSize());
        onMapWriteReady(true, this, data);
    }
}

void VKBuffer::CallMapReadCallback(std::uint32_t offset, std::uint32_t size)
{
    if (!onMapReadReady_)
    {
        LOGW("VKBuffer::CallMapReadCallback: has been reset.");
        return ;
    }
    
    auto callback = onMapReadReady_;
    onMapReadReady_ = nullptr;
    
    if (!IsDisposed())
    {
        const void* data = MapRead(offset, size);
        callback(true, this, data);
    }
    else
    {
        callback(false, this, nullptr);
    }
}

void VKBuffer::CallMapWriteCallback(std::uint32_t offset, std::uint32_t size)
{
    if (!onMapWriteReady_)
    {
        LOGW("VKBuffer::CallMapWriteCallback: has been reset.");
        return ;
    }
    
    auto callback = onMapWriteReady_;
    onMapWriteReady_ = nullptr;
    
    if (!IsDisposed())
    {
        void* data = MapWrite(offset, size);
        callback(true, this, data);
    }
    else
    {
        callback(false, this, nullptr);
    }
}

void VKBuffer::Unmap() const
{
    GFX_ASSERT(pData_);
#if USE_VULKAN_MEMORY_ALLCATOR
    vmaUnmapMemory(VKDEVICE()->GetVmaAllocator(), vmaAllocation_);
#else
    vkUnmapMemory(VKDEVICE()->GetNative(), vkBufferMemory_);
#endif
    pData_ = nullptr;
}

void VKBuffer::Destroy()
{
    Dispose();
}

void VKBuffer::SetSubData(std::uint32_t offset, std::uint32_t byteSize, const void* data) const
{
    GFX_ASSERT(byteSize > 0);
    auto pAddress = (std::uint8_t*) MapWrite(offset, byteSize);
    GFX_ASSERT(pAddress);
    memcpy(pAddress, data, byteSize);
    Unmap();
}

NS_GFX_END
