//
// Created by realxie on 2020-02-25.
//

#ifndef PROJ_ANDROID_FRAMEBUFFER_H
#define PROJ_ANDROID_FRAMEBUFFER_H

#include "RHIObjectBase.h"

NS_RHI_BEGIN

class Framebuffer : public RHIObjectBase
{
public:
    Framebuffer(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::Framebuffer)
    {}

protected:
    virtual ~Framebuffer() = default;
};

NS_RHI_END

#endif //PROJ_ANDROID_FRAMEBUFFER_H
