//
// Created by realxie on 2019-10-04.
//

#ifndef ALITA_VKSHADER_H
#define ALITA_VKSHADER_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKShader : public Shader
{
protected:
    VKShader() = default;
    
    bool Init(VKDevice* device, const ShaderModuleDescriptor &descriptor);

public:
    static VKShader* Create(VKDevice* device, const ShaderModuleDescriptor &descriptor);
    
    virtual ~VKShader();
    
    VkShaderModule GetNative() const
    { return vkShaderModule_; }

private:
    VkDevice vkDevice_ = nullptr;
    VkShaderModule vkShaderModule_ = 0;
};

NS_RHI_END

#endif //ALITA_VKSHADER_H
