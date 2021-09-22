//
// Created by realxie on 2019-10-11.
//

#include "VKRenderQueue.h"

NS_RHI_BEGIN

VKRenderQueue::VKRenderQueue(VKDevice* device)
    : RenderQueue(device)
{
}

void VKRenderQueue::Dispose()
{
    RHI_DISPOSE_BEGIN();
    
    RHI_DISPOSE_END();
}

VKRenderQueue::~VKRenderQueue()
{
    Dispose();
}

NS_RHI_END