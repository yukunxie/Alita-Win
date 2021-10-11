//
// Created by realxie on 2020/8/24.
//

#include "GFX/Device.h"

NS_GFX_BEGIN

void Device::AddAutoReleaseObjectToPool(GfxBase* object)
{
    RHI_ASSERT(object);
    if (!object) return;
#if RHI_DEBUG
    autoReleasedObjectPool_.emplace(object->GetObjectType(), object);
#else
    autoReleasedObjectPool_.push_back(object);
#endif
}

void Device::PurgeAutoReleasePool()
{
    int index = 0;
#if RHI_DEBUG
    for (auto it : autoReleasedObjectPool_)
    {
        RHI_SAFE_RELEASE(it.second);
        index++;
    }
#else
    for (auto it : autoReleasedObjectPool_)
    {
        RHI_SAFE_RELEASE(it);
    }
#endif
    
    autoReleasedObjectPool_.clear();
}

NS_GFX_END