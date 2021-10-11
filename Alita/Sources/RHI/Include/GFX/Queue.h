//
// Created by realxie on 2019-10-18.
//

#ifndef RHI_QUEUE_H
#define RHI_QUEUE_H

#include "Macros.h"
#include "GFXBase.h"
#include "CommandBuffer.h"
#include "Fence.h"

NS_GFX_BEGIN

class Queue : public GfxBase
{
public:
    Queue(Device* GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::Queue)
    {}

protected:
    virtual ~Queue() = default;

public:
    virtual void Submit(std::uint32_t commandBufferCount, CommandBuffer* const* commandBuffers) = 0;
    
    virtual Fence* CreateFence(const FenceDescriptor &descriptor) = 0;
    
    virtual void Signal(Fence* fence, std::uint64_t signalValue) = 0;
    
    virtual void WaitQueueIdle() = 0;
};

NS_GFX_END

#endif //RHI_QUEUE_H
