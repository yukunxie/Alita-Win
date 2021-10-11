//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_SHADER_H
#define RHI_SHADER_H


#include "Macros.h"
#include "GFXBase.h"

NS_GFX_BEGIN

class Shader : public GfxBase
{
public:
    Shader(Device* GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::Shader)
    {
    }

protected:
    virtual ~Shader() = default;
};

NS_GFX_END

#endif //RHI_SHADER_H
