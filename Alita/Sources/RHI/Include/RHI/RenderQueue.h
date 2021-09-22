//
// Created by realxie on 2019-10-11.
//

#ifndef RHI_RENDERQUEUE_H
#define RHI_RENDERQUEUE_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "CommandBuffer.h"

NS_RHI_BEGIN

class RenderQueue : public RHIObjectBase
{
public:
    RenderQueue(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::RenderQueue)
    {
    }

protected:
    virtual ~RenderQueue() = default;

public:
    virtual void Submit(const CommandBuffer* commandBuffer) = 0;
};

NS_RHI_END


#endif //RHI_RENDERQUEUE_H
