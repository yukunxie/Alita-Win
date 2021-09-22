//
// Created by realxie on 2020-03-24.
//

#ifndef PROJ_ANDROID_VKRENDERBUNDLE_H
#define PROJ_ANDROID_VKRENDERBUNDLE_H


#include "VKDevice.h"

NS_RHI_BEGIN

class VKRenderBundle : public RenderBundle
{
protected:
    VKRenderBundle(VKDevice* device);
    
    friend class VKDevice;
    
public:
    bool Init();
};

NS_RHI_END

#endif //PROJ_ANDROID_VKRENDERBUNDLE_H
