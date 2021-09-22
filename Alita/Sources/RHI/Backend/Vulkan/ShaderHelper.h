//
// Created by realxie on 2019-10-28.
//

#ifndef RHI_SHADERHELPER_H
#define RHI_SHADERHELPER_H

#include "VulkanMacros.h"

#include <string>

NS_RHI_BEGIN

enum class ShaderType
{
    VERTEX,
    FRAGMENT,
    COMPUTE
};

std::vector<std::uint32_t> CompileGLSLToSPIRV(const std::string& code, ShaderType type, const std::string& includeSearchPath = "");

std::vector<std::uint32_t> CompileGLSLToSPIRV(const char* code, ShaderType type, const std::string& includeSearchPath = "");

NS_RHI_END


#endif //RHI_SHADERHELPER_H
