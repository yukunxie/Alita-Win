//
// Created by realxie on 2019-10-18.
//

#ifndef RHI_QUEUE_H
#define RHI_QUEUE_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "CommandBuffer.h"
#include "Fence.h"

NS_RHI_BEGIN

class Queue : public RHIObjectBase
{
public:
    Queue(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::Queue)
    {}

protected:
    virtual ~Queue() = default;

public:
    virtual void Submit(std::uint32_t commandBufferCount, CommandBuffer* const* commandBuffers) = 0;
    
    virtual Fence* CreateFence(const FenceDescriptor &descriptor) = 0;
    
    virtual void Signal(Fence* fence, std::uint64_t signalValue) = 0;
    
    virtual void WaitQueueIdle() = 0;
};

NS_RHI_END

#endif //RHI_QUEUE_H
