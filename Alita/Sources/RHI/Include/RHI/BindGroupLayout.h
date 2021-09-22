//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_BINDGROUPLAYOUT_H
#define RHI_BINDGROUPLAYOUT_H

#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class BindGroupLayout : public RHIObjectBase
{
public:
    BindGroupLayout(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::BindGroupLayout)
    {}

protected:
    virtual ~BindGroupLayout() = default;
};

NS_RHI_END

#endif //RHI_BINDGROUPLAYOUT_H
