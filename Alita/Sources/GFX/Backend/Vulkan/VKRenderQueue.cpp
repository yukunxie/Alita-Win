//
// Created by realxie on 2019-10-11.
//

#include "VKRenderQueue.h"

NS_GFX_BEGIN

VKRenderQueue::VKRenderQueue(VKDevice* device)
    : RenderQueue(device)
{
}

void VKRenderQueue::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    GFX_DISPOSE_END();
}

VKRenderQueue::~VKRenderQueue()
{
    Dispose();
}

NS_GFX_END