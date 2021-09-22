//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_SHADER_H
#define RHI_SHADER_H


#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class Shader : public RHIObjectBase
{
public:
    Shader(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::Shader)
    {
    }

protected:
    virtual ~Shader() = default;
};

NS_RHI_END

#endif //RHI_SHADER_H
