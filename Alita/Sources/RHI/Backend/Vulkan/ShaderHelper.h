//
// Created by realxie on 2019-10-28.
//

#ifndef ALITA_SHADERHELPER_H
#define ALITA_SHADERHELPER_H

#include "VulkanMarcos.h"

#include <string>
#include <vector>

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


#endif //ALITA_SHADERHELPER_H
