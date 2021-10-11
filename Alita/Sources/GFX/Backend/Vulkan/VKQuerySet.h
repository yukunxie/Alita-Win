//
// Created by realxie on 2020-04-09.
//

#ifndef PROJ_ANDROID_VKQUERYSET_H
#define PROJ_ANDROID_VKQUERYSET_H

#include "VKDevice.h"

NS_GFX_BEGIN

class VKQuerySet : public QuerySet
{
public:
    VKQuerySet(VKDevice* device);
    
    bool Init(const QuerySetDescriptor &descriptor);
    
    VkQueryPool GetNative() const
    { return vkQueryPool_; }
    
    virtual void Dispose() override;

protected:
    ~VKQuerySet();

private:
    VkQueryPool vkQueryPool_ = VK_NULL_HANDLE;
};

NS_GFX_END


#endif //PROJ_ANDROID_VKQUERYSET_H
