//
// Created by realxie on 2019-10-18.
//

#ifndef ALITA_QUEUE_H
#define ALITA_QUEUE_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "CommandBuffer.h"

NS_RHI_BEGIN

class Queue : public RHIObjectBase
{
public:
    virtual ~Queue() = default;

public:
    virtual void Submit(CommandBuffer* commandBuffer) = 0;
    
    //    GPUFence createFence(optional GPUFenceDescriptor descriptor = {});
    //    void signal(GPUFence fence, unsigned long long signalValue
};

NS_RHI_END

#endif //ALITA_QUEUE_H
