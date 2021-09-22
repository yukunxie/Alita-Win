//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_BINDGROUP_H
#define RHI_BINDGROUP_H

#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class BindGroup : public RHIObjectBase
{
public:
    BindGroup(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::BindGroup)
    {
    }
    
protected:
    virtual ~BindGroup() = default;
};

NS_RHI_END

#endif //RHI_BINDGROUP_H
