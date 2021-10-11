//
// Created by realxie on 2019-10-02.
//

#ifndef RHI_VKBUFFER_H
#define RHI_VKBUFFER_H

#include "VKDevice.h"

NS_GFX_BEGIN

class VKBuffer final : public Buffer
{
protected:
    VKBuffer(VKDevice* device);
    
    virtual ~VKBuffer();

public:
    
    bool Init(const BufferDescriptor &descriptor);
    
    VkBuffer GetNative() const
    { return vkBuffer_; }
    
    void UpdateBuffer(const void* data, std::uint32_t offset, std::uint32_t size);
    
    virtual const void* MapRead(std::uint32_t offset, std::uint32_t size) const override;
    
    virtual void* MapWrite(std::uint32_t offset, std::uint32_t size) const override;
    
    virtual void
    MapReadAsync(const std::function<void(bool, Buffer*, const void*)> &onMapReadReady, 
        std::uint32_t offset = 0, std::uint32_t size = 0) override;
    
    virtual void
    MapWriteAsync(const std::function<void(bool, Buffer*, void*)> &onMapWriteReady, 
        std::uint32_t offset = 0, std::uint32_t size = 0) override;
    
    virtual void Unmap() const override;
    
    virtual void Destroy() override;
    
    virtual void
    SetSubData(std::uint32_t offset, std::uint32_t byteSize, const void* data) const override;
    
    virtual void Dispose() override;
    
    void CallMapReadCallback(std::uint32_t offset = 0, std::uint32_t size = 0);
    
    void CallMapWriteCallback(std::uint32_t offset = 0, std::uint32_t size = 0);

private:
    VkBuffer vkBuffer_ = VK_NULL_HANDLE;
    mutable void* pData_ = nullptr;

#if USE_VULKAN_MEMORY_ALLCATOR
    VmaAllocation vmaAllocation_ = nullptr;
    VmaAllocationInfo vmaAllocationInfo_ = {};
#else
    VkDeviceMemory vkBufferMemory_ = VK_NULL_HANDLE;
#endif
    
    std::function<void(bool, Buffer*, const void*)> onMapReadReady_ = nullptr;
    std::function<void(bool, Buffer*, void*)> onMapWriteReady_ = nullptr;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKBUFFER_H
