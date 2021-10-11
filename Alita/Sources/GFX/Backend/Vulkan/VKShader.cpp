//
// Created by realxie on 2019-10-04.
//

#include "VKShader.h"

#include "glslang/Public/ShaderLang.h"
#include "SPIRV/GlslangToSpv.h"
#include "StandAlone/DirStackFileIncluder.h"

NS_GFX_BEGIN

VKShader::VKShader(VKDevice* device)
    : Shader(device)
{
}

void VKShader::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    if (vkShaderModule_)
    {
        vkDestroyShaderModule(VKDEVICE()->GetNative(), vkShaderModule_, nullptr);
        vkShaderModule_ = VK_NULL_HANDLE;
    }
    
    GFX_DISPOSE_END();
}

VKShader::~VKShader()
{
    Dispose();
}

bool VKShader::Init(const ShaderModuleDescriptor &descriptor)
{
    if (descriptor.codeType == ShaderCodeType::BINARY)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = descriptor.code.size();
        createInfo.pCode = reinterpret_cast<const std::uint32_t*>(descriptor.code.data());
        
        CALL_VK(
            vkCreateShaderModule(GFX_CAST(VKDevice*, GetGPUDevice())->GetNative(), &createInfo, nullptr, &vkShaderModule_));
    }
    
    return VK_NULL_HANDLE != vkShaderModule_;
}

NS_GFX_END