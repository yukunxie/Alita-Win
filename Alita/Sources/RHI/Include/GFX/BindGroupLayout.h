//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_BINDGROUPLAYOUT_H
#define RHI_BINDGROUPLAYOUT_H

#include "Macros.h"
#include "GFXBase.h"

NS_GFX_BEGIN

class BindGroupLayout : public GfxBase
{
public:
    BindGroupLayout(Device* GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::BindGroupLayout)
    {}

protected:
    virtual ~BindGroupLayout() = default;
};

NS_GFX_END

#endif //RHI_BINDGROUPLAYOUT_H
