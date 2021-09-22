//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_VKSHADER_H
#define RHI_VKSHADER_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKShader final : public Shader
{
protected:
    VKShader(VKDevice* device);
    
public:
    
    bool Init(const ShaderModuleDescriptor &descriptor);
    
    virtual ~VKShader();
    
    VkShaderModule GetNative() const
    { return vkShaderModule_; }
    
    virtual void Dispose() override;

private:
    VkShaderModule vkShaderModule_ = VK_NULL_HANDLE;
    
    friend class VKDevice;
};

NS_RHI_END

#endif //RHI_VKSHADER_H
