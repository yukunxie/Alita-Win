//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_RENDERPASS_H
#define RHI_RENDERPASS_H

#include "Macros.h"
#include "GFXBase.h"

NS_GFX_BEGIN

class RenderPass : public GfxBase
{
public:
    RenderPass(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::RenderPass)
    {}

    virtual ~RenderPass() = default;
};

NS_GFX_END

#endif //RHI_RENDERPASS_H
