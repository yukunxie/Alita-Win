//
// Created by realxie on 2019-10-02.
//

#ifndef ALITA_BUFFER_H
#define ALITA_BUFFER_H

#include "Macros.h"
#include "BindingResource.h"
#include "Descriptors.h"

NS_RHI_BEGIN

class Buffer : public RHIObjectBase
{
protected:
    virtual ~Buffer()
    {}

public:
    virtual const void* MapReadAsync(std::uint32_t offset, std::uint32_t size) = 0;
    
    virtual void* MapWriteAsync(std::uint32_t offset, std::uint32_t size) = 0;
    
    virtual const void* MapReadAsync() = 0;
    
    virtual void* MapWriteAsync() = 0;
    
    virtual void Unmap() = 0;
    
    virtual void Destroy() = 0;
    
    virtual void SetSubData(std::uint32_t offset, std::uint32_t byteSize, const void* data) = 0;
};

struct BufferBinding : public BindingResource
{
    BufferBinding()
        : BindingResource(BindingResourceType::BUFFER)
    {}
    
    BufferBinding(const Buffer* buffer, BufferSize offset, BufferSize size)
        : BindingResource(BindingResourceType::BUFFER), buffer(buffer), offset(offset), size(size)
    {}
    
    const Buffer* buffer = nullptr;
    BufferSize offset = 0;
    BufferSize size   = std::numeric_limits<BufferSize>::max();
};

NS_RHI_END

#endif //ALITA_BUFFER_H
