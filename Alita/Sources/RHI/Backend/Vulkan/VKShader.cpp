//
// Created by realxie on 2019-10-04.
//

#include "VKShader.h"

#include "glslang/Public/ShaderLang.h"
#include "SPIRV/GlslangToSpv.h"
#include "StandAlone/DirStackFileIncluder.h"

NS_RHI_BEGIN

bool glslangInitialized = false;

const char* shaderString = "#version 450\n"
                           "#extension GL_ARB_separate_shader_objects : enable\n"
                           "#extension GL_ARB_shading_language_450pack : enable\n"
                           "\n"
                           "layout(location = 0) in vec3 inPosition;\n"
                           "layout(location = 1) in vec3 inColor;\n"
                           "layout(location = 2) in vec2 inTexCoord;\n"
                           "\n"
                           "layout(binding = 0) uniform UniformBufferObject {\n"
                           "    mat4 model;\n"
                           "    mat4 view;\n"
                           "    mat4 proj;\n"
                           "} ubo;\n"
                           "\n"
                           "layout(location = 0) out vec3 fragColor;\n"
                           "layout(location = 1) out vec2 texCoord;\n"
                           "\n"
                           "void main() {\n"
                           "    // gl_Position = vec4(inPosition, 1.0);\n"
                           "    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);\n"
                           "    fragColor = inColor;\n"
                           "    texCoord  = inTexCoord;\n"
                           "}";

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

bool VKShader::Init(VKDevice* device, const ShaderModuleDescriptor &descriptor)
{
    //    if (!glslangInitialized)
    //    {
    //        glslang::InitializeProcess();
    //        glslangInitialized = true;
    //    }
    //
    //    if (glslangInitialized)
    //    {
    //        auto ShaderType = EShLanguage::EShLangVertex;
    //        glslang::TShader shader(ShaderType);
    //        shader.setStrings(&shaderString, 1);
    //
    //        int ClientInputSemanticsVersion = 100; // maps to, say, #define VULKAN 100
    //        glslang::EShTargetClientVersion VulkanClientVersion = glslang::EShTargetVulkan_1_0;
    //        glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_0;
    //
    //        shader.setEnvInput(glslang::EShSourceGlsl, ShaderType, glslang::EShClientVulkan, ClientInputSemanticsVersion);
    //        shader.setEnvClient(glslang::EShClientVulkan, VulkanClientVersion);
    //        shader.setEnvTarget(glslang::EShTargetSpv, TargetVersion);
    //
    //        TBuiltInResource Resources;
    //        Resources = DefaultTBuiltInResource;
    //        EShMessages messages = (EShMessages) (EShMsgSpvRules | EShMsgVulkanRules);
    //
    //        const int DefaultVersion = 100;
    //
    //        DirStackFileIncluder Includer;
    //
    //        //Get Path of File
    ////        std::string Path = GetFilePath(filename);
    //        Includer.pushExternalLocalDirectory("");
    //
    //        std::string PreprocessedGLSL;
    //
    //        if (!shader.preprocess(&Resources, DefaultVersion, ENoProfile, false, false, messages, &PreprocessedGLSL, Includer))
    //        {
    //            auto log = shader.getInfoLog();
    //            auto dlog = shader.getInfoDebugLog();
    //            LOGE("xxxxxxxx, %s, %s", log, dlog);
    //        }
    //
    //        const char* PreprocessedCStr = PreprocessedGLSL.c_str();
    //        shader.setStrings(&PreprocessedCStr, 1);
    //
    //        if (!shader.parse(&Resources, 100, false, messages))
    //        {
    //            LOGE("xxxxxxxx");
    //        }
    //
    //        glslang::TProgram Program;
    //        Program.addShader(&shader);
    //
    //        if(!Program.link(messages))
    //        {
    //            LOGE("xxxxxxxx");
    //        }
    //
    //        RHI_ASSERT(Program.buildReflection());
    //
    //        auto count = Program.getNumUniformVariables();
    //        for (int i = 0; i < count; i++)
    //        {
    //            auto binding = Program.getUniformBinding(i);
    //            auto name = Program.getUniformBlockName(i);
    //            auto name2 = Program.getUniformName(i);
    //            auto offset = Program.getUniformBufferOffset(i);
    //            auto type = Program.getUniformTType(i);
    //            auto size = Program.getUniformBlockSize(i);
    //            int k = 10;
    //        }
    //
    //        std::vector<unsigned int> SpirV;
    //        spv::SpvBuildLogger logger;
    //        glslang::SpvOptions spvOptions;
    //        glslang::GlslangToSpv(*Program.getIntermediate(ShaderType), SpirV, &logger, &spvOptions);
    //
    //        LOGI("kkkkkk");
    //
    //    }
    
    vkDevice_ = device->GetDevice();
    
    if (descriptor.codeType == ShaderCodeType::BINARY)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = descriptor.binaryCode.size();
        createInfo.pCode = reinterpret_cast<const std::uint32_t*>(descriptor.binaryCode.data());
        
        CALL_VK(vkCreateShaderModule(vkDevice_, &createInfo, nullptr, &vkShaderModule_));
        return true;
    }
    else
    {
        return false;
    }
}

VKShader* VKShader::Create(VKDevice* device, const ShaderModuleDescriptor &descriptor)
{
    auto ret = new VKShader();
    if (ret && ret->Init(device, descriptor))
    {
        RHI_SAFE_RETAIN(ret);
        return ret;
    }
    if (ret) delete ret;
    return nullptr;
}

VKShader::~VKShader()
{
    if (vkShaderModule_)
    {
        vkDestroyShaderModule(vkDevice_, vkShaderModule_, nullptr);
    }
}


NS_RHI_END