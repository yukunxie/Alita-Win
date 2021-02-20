//
// Created by realxie on 2019-10-02.
//

#ifndef ALITA_VKBUFFER_H
#define ALITA_VKBUFFER_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKBuffer : public Buffer
{
public:
    VKBuffer(VKDevice* device, const BufferDescriptor& descriptor);

    VkBuffer GetNative() const {return vkBuffer_;}

    void UpdateBuffer(const void* data, std::uint32_t offset, std::uint32_t size);

    virtual const void* MapReadAsync(std::uint32_t offset, std::uint32_t size) override;
    virtual void* MapWriteAsync(std::uint32_t offset, std::uint32_t size) override;
    virtual const void* MapReadAsync() override ;
    virtual void* MapWriteAsync() override ;
    virtual void Unmap() override;
    virtual void Destroy() override;
    virtual void SetSubData(std::uint32_t offset, std::uint32_t byteSize, const void* data) override ;

protected:
    virtual ~VKBuffer();

private:
    VkDevice        vkDevice_       = nullptr;
    VkBuffer        vkBuffer_       = 0;
    VkDeviceMemory  vkBufferMemory_ = 0;
    void*           pData_          = nullptr;

};

NS_RHI_END


#endif //ALITA_VKBUFFER_H
