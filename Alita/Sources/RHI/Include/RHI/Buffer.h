//
// Created by realxie on 2019-10-02.
//

#ifndef RHI_BUFFER_H
#define RHI_BUFFER_H

#include "Macros.h"
#include "BindingResource.h"
#include "Descriptors.h"

NS_RHI_BEGIN

class BufferBinding;

class Buffer : public RHIObjectBase
{
public:
    Buffer(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::Buffer)
    {}

protected:
    bool Init(const BufferDescriptor &descriptor)
    {
        bufferSize_ = descriptor.size;
        bufferUsage_ = descriptor.usage;
        return true;
    }
    
    virtual ~Buffer()
    {}

public:
    BufferSize GetBufferSize()
    { return bufferSize_; }
    
    BufferUsageFlags GetUsage()
    { return bufferUsage_; }

public:
    virtual const void* MapRead(std::uint32_t offset, std::uint32_t size) = 0;
    
    virtual void* MapWrite(std::uint32_t offset, std::uint32_t size) = 0;
    
    virtual void MapReadAsync(
        const std::function<void(bool, Buffer*, const void*)> &onMapReadReady, 
        std::uint32_t offset = 0, std::uint32_t size = 0) = 0;
    
    virtual void
    MapWriteAsync(const std::function<void(bool, Buffer*, void*)> &onMapWriteReady, 
        std::uint32_t offset = 0, std::uint32_t size = 0) = 0;
    
    virtual void Unmap() = 0;
    
    virtual void Destroy() = 0;
    
    virtual void SetSubData(std::uint32_t offset, std::uint32_t byteSize, const void* data) = 0;

    inline void increaseBindCount() { bindCount_++; }
    inline void reduceBindCount()  { bindCount_--; }
    inline int getBindCount()  { return bindCount_; }

protected:
    BufferSize bufferSize_ = 0;
    BufferUsageFlags bufferUsage_ = 0;

    int bindCount_ = 0;
};

class BufferBinding final : public BindingResource
{
public:
    BufferBinding() = delete;
    
    BufferBinding(Device* device)
        : BindingResource(device, BindingResourceType::Buffer)
    {
    }
    
    bool Init(Buffer* buffer_, BufferSize offset_, BufferSize size_)
    {
        buffer = buffer_;
        offset = offset_;
        size = size_;
        return true;
    }

protected:
    virtual ~BufferBinding() = default;

public:
    virtual void Dispose() override
    {}
    
    Buffer* GetBuffer()
    { return buffer.Get(); }
    
    BufferSize GetOffset()
    { return offset; }
    
    BufferSize GetSize()
    { return size; }

private:
    RHIObjectWrapper<Buffer> buffer;
    BufferSize offset = 0;
    BufferSize size = std::numeric_limits<BufferSize>::max();
};

NS_RHI_END

#endif //RHI_BUFFER_H
