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
    Framebuffer(Device* GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::Framebuffer)
    {}

protected:
    virtual ~Framebuffer() = default;
};

NS_GFX_END

#endif //PROJ_ANDROID_FRAMEBUFFER_H
