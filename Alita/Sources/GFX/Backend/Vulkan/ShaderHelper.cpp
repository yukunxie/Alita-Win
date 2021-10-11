//
// Created by realxie on 2019-10-28.
//

#include "ShaderHelper.h"

#include "GFX/GFX.h"

#include "glslang/Public/ShaderLang.h"
#include "SPIRV/GlslangToSpv.h"
#include "StandAlone/DirStackFileIncluder.h"
#include <regex>

NS_GFX_BEGIN

static bool glslangInitialized = false;

const TBuiltInResource DefaultTBuiltInResource = {
    /* .MaxLights = */ 32,
    /* .MaxClipPlanes = */ 6,
    /* .MaxTextureUnits = */ 32,
    /* .MaxTextureCoords = */ 32,
    /* .MaxVertexAttribs = */ 64,
    /* .MaxVertexUniformComponents = */ 4096,
    /* .MaxVaryingFloats = */ 64,
    /* .MaxVertexTextureImageUnits = */ 32,
    /* .MaxCombinedTextureImageUnits = */ 80,
    /* .MaxTextureImageUnits = */ 32,
    /* .MaxFragmentUniformComponents = */ 4096,
    /* .MaxDrawBuffers = */ 32,
    /* .MaxVertexUniformVectors = */ 128,
    /* .MaxVaryingVectors = */ 8,
    /* .MaxFragmentUniformVectors = */ 16,
    /* .MaxVertexOutputVectors = */ 16,
    /* .MaxFragmentInputVectors = */ 15,
    /* .MinProgramTexelOffset = */ -8,
    /* .MaxProgramTexelOffset = */ 7,
    /* .MaxClipDistances = */ 8,
    /* .MaxComputeWorkGroupCountX = */ 65535,
    /* .MaxComputeWorkGroupCountY = */ 65535,
    /* .MaxComputeWorkGroupCountZ = */ 65535,
    /* .MaxComputeWorkGroupSizeX = */ 1024,
    /* .MaxComputeWorkGroupSizeY = */ 1024,
    /* .MaxComputeWorkGroupSizeZ = */ 64,
    /* .MaxComputeUniformComponents = */ 1024,
    /* .MaxComputeTextureImageUnits = */ 16,
    /* .MaxComputeImageUniforms = */ 8,
    /* .MaxComputeAtomicCounters = */ 8,
    /* .MaxComputeAtomicCounterBuffers = */ 1,
    /* .MaxVaryingComponents = */ 60,
    /* .MaxVertexOutputComponents = */ 64,
    /* .MaxGeometryInputComponents = */ 64,
    /* .MaxGeometryOutputComponents = */ 128,
    /* .MaxFragmentInputComponents = */ 128,
    /* .MaxImageUnits = */ 8,
    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .MaxCombinedShaderOutputResources = */ 8,
    /* .MaxImageSamples = */ 0,
    /* .MaxVertexImageUniforms = */ 0,
    /* .MaxTessControlImageUniforms = */ 0,
    /* .MaxTessEvaluationImageUniforms = */ 0,
    /* .MaxGeometryImageUniforms = */ 0,
    /* .MaxFragmentImageUniforms = */ 8,
    /* .MaxCombinedImageUniforms = */ 8,
    /* .MaxGeometryTextureImageUnits = */ 16,
    /* .MaxGeometryOutputVertices = */ 256,
    /* .MaxGeometryTotalOutputComponents = */ 1024,
    /* .MaxGeometryUniformComponents = */ 1024,
    /* .MaxGeometryVaryingComponents = */ 64,
    /* .MaxTessControlInputComponents = */ 128,
    /* .MaxTessControlOutputComponents = */ 128,
    /* .MaxTessControlTextureImageUnits = */ 16,
    /* .MaxTessControlUniformComponents = */ 1024,
    /* .MaxTessControlTotalOutputComponents = */ 4096,
    /* .MaxTessEvaluationInputComponents = */ 128,
    /* .MaxTessEvaluationOutputComponents = */ 128,
    /* .MaxTessEvaluationTextureImageUnits = */ 16,
    /* .MaxTessEvaluationUniformComponents = */ 1024,
    /* .MaxTessPatchComponents = */ 120,
    /* .MaxPatchVertices = */ 32,
    /* .MaxTessGenLevel = */ 64,
    /* .MaxViewports = */ 16,
    /* .MaxVertexAtomicCounters = */ 0,
    /* .MaxTessControlAtomicCounters = */ 0,
    /* .MaxTessEvaluationAtomicCounters = */ 0,
    /* .MaxGeometryAtomicCounters = */ 0,
    /* .MaxFragmentAtomicCounters = */ 8,
    /* .MaxCombinedAtomicCounters = */ 8,
    /* .MaxAtomicCounterBindings = */ 1,
    /* .MaxVertexAtomicCounterBuffers = */ 0,
    /* .MaxTessControlAtomicCounterBuffers = */ 0,
    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .MaxGeometryAtomicCounterBuffers = */ 0,
    /* .MaxFragmentAtomicCounterBuffers = */ 1,
    /* .MaxCombinedAtomicCounterBuffers = */ 1,
    /* .MaxAtomicCounterBufferSize = */ 16384,
    /* .MaxTransformFeedbackBuffers = */ 4,
    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
    /* .MaxCullDistances = */ 8,
    /* .MaxCombinedClipAndCullDistances = */ 8,
    /* .MaxSamples = */ 4,
    /* .maxMeshOutputVerticesNV = */ 256,
    /* .maxMeshOutputPrimitivesNV = */ 512,
    /* .maxMeshWorkGroupSizeX_NV = */ 32,
    /* .maxMeshWorkGroupSizeY_NV = */ 1,
    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
    /* .maxTaskWorkGroupSizeX_NV = */ 32,
    /* .maxTaskWorkGroupSizeY_NV = */ 1,
    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
    /* .maxMeshViewCountNV = */ 4,
    
    /* .limits = */ {
                           /* .nonInductiveForLoops = */ 1,
                           /* .whileLoops = */ 1,
                           /* .doWhileLoops = */ 1,
                           /* .generalUniformIndexing = */ 1,
                           /* .generalAttributeMatrixVectorIndexing = */ 1,
                           /* .generalVaryingIndexing = */ 1,
                           /* .generalSamplerIndexing = */ 1,
                           /* .generalVariableIndexing = */ 1,
                           /* .generalConstantMatrixVectorIndexing = */ 1,
                       }};

std::vector<std::uint32_t>
CompileGLSLToSPIRV(const std::string &code, ShaderType type, const std::string &includeSearchPath)
{
    return CompileGLSLToSPIRV(code.c_str(), type, includeSearchPath);
}

std::vector<std::uint32_t>
CompileGLSLToSPIRV(const char* source, ShaderType type, const std::string &includeSearchPath)
{
    extern bool gIsDeviceSupportNegativeViewport;
    std::string vertexShaderCodeTmp(source);
    //if (ShaderType::VERTEX == type && !gIsDeviceSupportNegativeViewport)
    //{
    //    // Vulkan的NDC与openGL是反的，将y进行翻转即可
    //    const static std::regex mainPattern("void\\s+main\\s*\\(\\s*(void)?\\s*\\)");
    //    const char* shaderEntry = R"(
    //            void main()
    //            {
    //                _Main_Vulkan_VS();
    //                gl_Position.y *= -1.0;
    //            })";
    //    vertexShaderCodeTmp = std::regex_replace(source, mainPattern, "void _Main_Vulkan_VS()") + shaderEntry;
    //}
    //
    //const static std::regex versionPattern("#version.+\\n");
    //vertexShaderCodeTmp = std::regex_replace(vertexShaderCodeTmp, versionPattern, "");
    //
    //std::string shaderHeader = "#version 450\n#define USE_VULKAN 1\n";
    //
    //const static std::regex floatPrecisionPattern("precision\\s+(?:highp|lowp|mediump)\\s+float");
    //if (!std::regex_search(vertexShaderCodeTmp, floatPrecisionPattern))
    //{
    //    shaderHeader += "precision highp float;\n";
    //}
    //
    //const static std::regex samplerPrecisionPattern("precision\\s+(?:highp|lowp|mediump)\\s+sampler");
    //if (!std::regex_search(vertexShaderCodeTmp, samplerPrecisionPattern))
    //{
    //    shaderHeader += "precision highp sampler;\n";
    //}
    //
    //vertexShaderCodeTmp = shaderHeader + vertexShaderCodeTmp;
    
    const char* glslShaderCode = vertexShaderCodeTmp.c_str();
    
    RHI_SCOPED_PROFILING_GUARD("CompileGLSLToSPIRV");
    
    if (!glslangInitialized)
    {
        glslang::InitializeProcess();
        glslangInitialized = true;
    }
    
    auto shaderType = EShLanguage::EShLangVertex;
    
    switch (type)
    {
        case ShaderType::VERTEX:
            shaderType = EShLanguage::EShLangVertex;
            break;
        
        case ShaderType::FRAGMENT:
            shaderType = EShLanguage::EShLangFragment;
            break;
        
        case ShaderType::COMPUTE:
            shaderType = EShLanguage::EShLangCompute;
            break;
    }
    
    glslang::TShader shader(shaderType);
    shader.setStrings(&glslShaderCode, 1);
    
    int ClientInputSemanticsVersion = 100; // maps to, say, #define VULKAN 100
    glslang::EShTargetClientVersion VulkanClientVersion = glslang::EShTargetVulkan_1_0;
    glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_0;
    
    shader.setEnvInput(glslang::EShSourceGlsl, shaderType, glslang::EShClientVulkan,
                       ClientInputSemanticsVersion);
    shader.setEnvClient(glslang::EShClientVulkan, VulkanClientVersion);
    shader.setEnvTarget(glslang::EShTargetSpv, TargetVersion);
    
    TBuiltInResource Resources;
    Resources = DefaultTBuiltInResource;
    EShMessages messages = (EShMessages) (EShMsgSpvRules | EShMsgVulkanRules);
    
    const int DefaultVersion = 100;
    
    DirStackFileIncluder Includer;
    Includer.pushExternalLocalDirectory(includeSearchPath);
    
    std::string PreprocessedGLSL;
    
    if (!shader.preprocess(&Resources, DefaultVersion, ENoProfile, false, false, messages,
                           &PreprocessedGLSL, Includer))
    {
        LOGE("CompileGLSLToSPIRV fail[preprocess]: %s, %s", shader.getInfoLog(),
             shader.getInfoDebugLog());
    }
    
    const char* PreprocessedCStr = PreprocessedGLSL.c_str();
    shader.setStrings(&PreprocessedCStr, 1);
    
    if (!shader.parse(&Resources, 100, false, messages))
    {
        LOGE("CompileGLSLToSPIRV fail[parse]: %s, %s", shader.getInfoLog(),
             shader.getInfoDebugLog());
    }
    
    glslang::TProgram Program;
    Program.addShader(&shader);
    
    if (!Program.link(messages))
    {
        LOGE("CompileGLSLToSPIRV fail[link]");
    }
    
    std::vector<unsigned int> spirVShader;
    spv::SpvBuildLogger logger;
    glslang::SpvOptions spvOptions;
    glslang::GlslangToSpv(*Program.getIntermediate(shaderType), spirVShader, &logger, &spvOptions);
    
    return spirVShader;
}

NS_GFX_END