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
    BindGroup(Device* GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::BindGroup)
    {
    }
    
protected:
    virtual ~BindGroup() = default;
};

NS_GFX_END

#endif //RHI_BINDGROUP_H
