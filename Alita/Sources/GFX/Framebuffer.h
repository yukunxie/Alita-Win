//
// Created by realxie on 2020-02-25.
//

#ifndef PROJ_ANDROID_FRAMEBUFFER_H
#define PROJ_ANDROID_FRAMEBUFFER_H

#include "GFXBase.h"

NS_GFX_BEGIN

class Framebuffer : public GfxBase
{
public:
    Framebuffer(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::Framebuffer)
    {}

    virtual ~Framebuffer() = default;
};

NS_GFX_END

#endif //PROJ_ANDROID_FRAMEBUFFER_H
