//
// Created by realxie on 2019-10-11.
//

#ifndef ALITA_RENDERQUEUE_H
#define ALITA_RENDERQUEUE_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "CommandBuffer.h"

NS_RHI_BEGIN

class RenderQueue : public RHIObjectBase
{
public:
    virtual ~RenderQueue()
    {
    }

public:
    virtual void Submit(const CommandBuffer* commandBuffer) = 0;
};

NS_RHI_END


#endif //ALITA_RENDERQUEUE_H
