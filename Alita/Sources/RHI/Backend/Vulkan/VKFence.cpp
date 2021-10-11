//
// Created by realxie on 2020-01-29.
//

#include "VKFence.h"
#include "VKCommandBuffer.h"

NS_GFX_BEGIN

VKFence::VKFence(VKDevice* device)
    : Fence(device)
{
}

void VKFence::Dispose()
{
    RHI_DISPOSE_BEGIN();
    
    RHI_DISPOSE_END();
}

VKFence::~VKFence()
{
    Dispose();
}

bool VKFence::Init(const FenceDescriptor& descriptor)
{
    signaledValue_ = descriptor.initialValue;
    return true;
}

NS_GFX_END