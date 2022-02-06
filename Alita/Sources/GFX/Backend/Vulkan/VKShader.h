//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_VKSHADER_H
#define RHI_VKSHADER_H

#include "VKDevice.h"

NS_GFX_BEGIN

class VKShader final : public Shader
{
protected:
    VKShader(const DevicePtr& device);
    
public:
    virtual ~VKShader();
    
    bool Init(const ShaderModuleDescriptor &descriptor);
    
    VkShaderModule GetNative() const
    {
        return vkShaderModule_;
    }
    
    virtual void Dispose() override;

private:
    VkShaderModule vkShaderModule_ = VK_NULL_HANDLE;
    
    friend class VKDevice;
};

NS_GFX_END

#endif //RHI_VKSHADER_H
