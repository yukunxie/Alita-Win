//
// Created by realxie on 2019-10-04.
//

#ifndef ALITA_SHADER_H
#define ALITA_SHADER_H


#include "Macros.h"
#include "RHIObjectBase.h"


NS_RHI_BEGIN

class Shader;

class Shader : public RHIObjectBase
{
public:
    virtual ~Shader()
    {
    }
};

using ShaderModule = Shader;

NS_RHI_END

#endif //ALITA_SHADER_H
