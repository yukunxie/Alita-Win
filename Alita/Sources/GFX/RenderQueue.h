//
// Created by realxie on 2019-10-11.
//

#ifndef RHI_RENDERQUEUE_H
#define RHI_RENDERQUEUE_H

#include "Macros.h"
#include "GFXBase.h"
#include "CommandBuffer.h"

NS_GFX_BEGIN

class RenderQueue : public GfxBase
{
public:
    RenderQueue(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::RenderQueue)
    {
    }

    virtual ~RenderQueue() = default;

public:
    virtual void Submit(const CommandBuffer* commandBuffer) = 0;
};

NS_GFX_END


#endif //RHI_RENDERQUEUE_H
