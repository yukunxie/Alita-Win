//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_BINDGROUP_H
#define RHI_BINDGROUP_H

#include "Macros.h"
#include "GFXBase.h"

NS_GFX_BEGIN

class BindGroup : public GfxBase
{
public:
    BindGroup(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::BindGroup)
    {
    }
    
public:
    virtual ~BindGroup() = default;
};

NS_GFX_END

#endif //RHI_BINDGROUP_H
