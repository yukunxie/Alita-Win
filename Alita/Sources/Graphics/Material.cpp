//
// Created by realxie on 2019-10-29.
//

#include "Material.h"
#include "Engine/Engine.h"
#include "Loaders/ImageLoader.h"
#include "RenderScene.h"

#include "Backend/Vulkan/ShaderHelper.h"

NS_RX_BEGIN

std::uint32_t GetInputAttributeLocation(VertexBufferAttriKind kind)
{
    switch (kind)
    {
    case VertexBufferAttriKind::POSITION:
        return IA_LOCATION_POSITION;
    case VertexBufferAttriKind::NORMAL:
        return IA_LOCATION_NORMAL;
    case VertexBufferAttriKind::TEXCOORD:
        return IA_LOCATION_TEXCOORD;
    case VertexBufferAttriKind::DIFFUSE:
        return IA_LOCATION_DIFFUSE;
    case VertexBufferAttriKind::TANGENT:
        return IA_LOCATION_TANGENT;
    case VertexBufferAttriKind::BINORMAL:
        return IA_LOCATION_BINORMAL;
    case VertexBufferAttriKind::BITANGENT:
        return IA_LOCATION_BITANGENT;
    case VertexBufferAttriKind::TEXCOORD2:
        return IA_LOCATION_TEXCOORD2;
    }
    Assert(false, "invalid kind");
}

std::uint32_t GetFormatSize(InputAttributeFormat format)
{
    switch (format)
    {
    case InputAttributeFormat::FLOAT:
        return 4;
    case InputAttributeFormat::FLOAT2:
        return 8;
    case InputAttributeFormat::FLOAT3:
        return 12;
    case InputAttributeFormat::FLOAT4:
        return 16;
    }
    Assert(false, "");
    return 0;
}

static const std::string sShaderExtensions =
"#version 450\n"
"#extension GL_ARB_separate_shader_objects : enable\n"
"#extension GL_ARB_shading_language_450pack : enable\n";

static const std::string sShaderGlobalConstantBuffer =
"layout(binding = 0) uniform Global	                            \n"
"{	                                                            \n"
"    vec4 EyePos; // camera's world position	                \n"
"    vec4 SunLight; // xyz direction	                        \n"
"    vec4 SunLightColor; // rgb as color, and a as intensity    \n"
"    mat4 ViewMatrix;	                                        \n"
"    mat4 ProjMatrix;	                                        \n"
"} uGlobal;                                                     \n";


static constexpr std::uint32_t _SimpleHash(const char* p)
{
    std::uint32_t hash = 0;
    for (; *p; p++)
    {
        hash = hash * 31 + *p;
    }
    return hash;
}

static RHI::Shader* _CreateShader(const std::string& filename, RHI::ShaderType shaderType, ETechniqueType techType, const std::vector<std::string>& userDefines = {})
{
    std::string data = FileSystem::GetInstance()->GetStringData(filename.c_str());

    std::string defines = "";
    for (const auto& d : userDefines)
    {
        defines += d + "\n";
    }

    std::string shaderText = sShaderExtensions
        + defines + "\n"
        + sShaderGlobalConstantBuffer + "\n"
        + data;

    std::string techEntryName = "";
    switch (techType)
    {
    case ETechniqueType::TShading:
        techEntryName = "void TShading()";
        break;
    case ETechniqueType::TGBufferGen:
        techEntryName = "void TGBufferGen()";
        break;
    case ETechniqueType::TShadowmapGen:
        techEntryName = "void TShadowmapGen()";
        break;
    default:
        RHI_ASSERT(false);
    }
    RHI_ASSERT(techEntryName.size() > 0);

    auto npos = shaderText.find(techEntryName.c_str(), 0);
    RHI_ASSERT(npos != std::string::npos);
    if (npos != std::string::npos)
    {
        shaderText = shaderText.replace(npos, techEntryName.size(), "void main()");
    }

    //strstr(shaderText.c_str())

    const std::vector<uint32>& spirV = RHI::CompileGLSLToSPIRV(shaderText, shaderType);

    //auto tmp = "E:\\Programs\\Alita-Win\\Alita\\x64\\Debug\\" + filename + ".spirv";
    //auto fd = fopen(tmp.c_str(), "wb");
    //fwrite(spirV.data(), sizeof(uint32), spirV.size(), fd);
    //fclose(fd);
    
    RHI::ShaderModuleDescriptor descriptor;
    {
        descriptor.code.resize(spirV.size() * sizeof(spirV[0]));
        memcpy(descriptor.code.data(), spirV.data(), descriptor.code.size());
        descriptor.codeType = RHI::ShaderCodeType::BINARY;
    }
   
    return Engine::GetGPUDevice()->CreateShaderModule(descriptor);
}

Material::Material(const std::string& configFilename)
{
    rapidjson::Document doc;
    {
        std::string text = FileSystem::GetInstance()->GetStringData(configFilename.c_str());
        doc.Parse(text.c_str());
    }

    vsFilename_ = doc["code"]["vs"].GetString();
    fsFilename_ = doc["code"]["fs"].GetString();

    ParseBindGroupLayout(doc);
}

void Material::Apply(const Pass* pass, ETechniqueType technique, ERenderSet renderSet, RHI::RenderPassEncoder& passEndcoder)
{
    // update binding buffer.
    {
        const GobalRenderParams& params = Engine::GetRenderScene()->GetGobalRenderParams();
        SetFloat("EyePos", 0, 3, (float*)&params.cameraWorldPosition.x);
        SetFloat("SunLight", 0, 3, (float*)&params.sunLightDirection.x);
        SetFloat("SunLightColor", 0, 4, (float*)&params.sunLightColor.x);
        SetFloat("ViewMatrix", 0, 16, (float*)&params.viewMatrix);
        SetFloat("ProjMatrix", 0, 16, (float*)&params.projMatrix);
    }

    PSOKey psoKey{};
    SetupPSOKey(psoKey, technique);
    SetupPSOKey(psoKey, renderSet);
    SetupPSOKey(psoKey, pass);

    RHI::RenderPipeline* pso = nullptr;

    if (rhiPSOMap_.contains(psoKey))
    {
        pso = rhiPSOMap_[psoKey];
    }
    else
    {
        ShaderSet shaders = CreateShaderSet(technique);
        pso = CreatePipelineState(psoKey, shaders);
        rhiPSOMap_[psoKey] = pso;
        RHI_SAFE_RETAIN(pso);
    }

    ApplyModifyToBindGroup(passEndcoder);
    passEndcoder.SetPipeline(pso);
    passEndcoder.SetBindGroup(0, rhiBindGroup_, 0, nullptr);
}

void Material::SetupPSOKey(PSOKey& psoKey, ERenderSet renderSet)
{
    switch (renderSet)
    {
    case rx::ERenderSet_Opaque:
        break;
    case rx::ERenderSet_Transparent:
        break;
    case rx::ERenderSet_PostProcess:
        psoKey.DepthWrite = 0;
        psoKey.DepthCmpFunc = (int)RHI::CompareFunction::ALWAYS;
        break;
    default:
        break;
    }
}

void Material::SetupPSOKey(PSOKey& psoKey, ETechniqueType technique)
{
    psoKey.Technique = (int)technique;
}

void Material::SetupPSOKey(PSOKey& psoKey, const Pass* pass)
{
    for (const auto& tp : pass->GetColorAttachments())
    {
        RHI_ASSERT(tp.first < kMaxAttachmentCount);
        psoKey.AttachmentFormats[tp.first] = (uint8)tp.second->GetFormat();
    }
    if (auto ds = pass->GetDSAttachment(); ds)
    {
        psoKey.DSAttachmentFormat = (uint8)ds->GetFormat();
    }
}

bool Material::SetFloat(const std::string& name, std::uint32_t offset, std::uint32_t count, float* data)
{
    const auto it = parameters_.find(name);
    if (it == parameters_.end())
        return false;

    const MaterialParameter& param = it->second;
    Assert(param.bindingObject->type == MaterailBindingObjectType::BUFFER, "");
    Assert(param.offset + offset + size < param.bindingObject->stride, "");
    param.bindingObject->buffer->SetSubData(param.offset + offset, count * sizeof(float), data);

    return true;
}

bool Material::SetTexture(const std::string& name, const RHI::Texture* texture)
{
    Assert(texture != nullptr, "null is invalid");
    const auto it = parameters_.find(name);
    if (it == parameters_.end())
        return false;
    const MaterialParameter& param = it->second;
    Assert(param.format == MaterialParameterType::TEXTURE2D, "invalid");

    if (param.bindingObject->texture == texture)
        return true;
    param.bindingObject->texture = texture;
    RHI_SAFE_RETAIN(param.bindingObject->texture);
    bBindingDirty_ = true;
    return true;
}

void Material::ApplyModifyToBindGroup(RHI::RenderPassEncoder& passEndcoder)
{
    if (!bBindingDirty_)
        return;

    bBindingDirty_ = false;
    RHI_SAFE_RELEASE(rhiBindGroup_);

    RHI::BindGroupDescriptor descriptor;
    descriptor.layout = rhiBindGroupLayout_;

    for (auto it : bindingObjects_)
    {
        if (it->type == MaterailBindingObjectType::BUFFER)
        {
            auto resource = Engine::GetGPUDevice()->CreateBufferBinding((RHI::Buffer*)it->buffer, 0, it->stride);
            RHI_SAFE_RETAIN(resource);
            RHI::BindGroupBinding tmp;
            {
                tmp.binding = it->binding;
                tmp.resource = resource;
            }
            descriptor.entries.push_back(tmp);
        }
        else if (it->type == MaterailBindingObjectType::TEXTURE2D)
        {
            auto resource = Engine::GetGPUDevice()->CreateTextureViewBinding(((RHI::Texture*)it->texture)->CreateView({}));
            RHI_SAFE_RETAIN(resource);
            RHI::BindGroupBinding tmp;
            {
                tmp.binding = it->binding;
                tmp.resource = resource;
            }
            descriptor.entries.push_back(tmp);
        }
        else if (it->type == MaterailBindingObjectType::SAMPLER2D)
        {
            auto resource = Engine::GetGPUDevice()->CreateSamplerBinding((RHI::Sampler*)it->sampler);
            RHI_SAFE_RETAIN(resource);
            RHI::BindGroupBinding tmp;
            {
                tmp.binding = it->binding;
                tmp.resource = resource;
            }
            descriptor.entries.push_back(tmp);
        }
        else
            Assert(false, "");
    }

    rhiBindGroup_ = Engine::GetGPUDevice()->CreateBindGroup(descriptor);
}

void Material::ParseBindGroupLayout(const rapidjson::Document& doc)
{
    if (!doc.HasMember("bindings"))
    {
        return;
    }

    for (auto& cfg : doc["bindings"].GetArray())
    {
        std::string type = cfg["type"].GetString();
        MaterialBindingObject* bindingObject = new MaterialBindingObject();
        bindingObject->stride = 0;
        bindingObjects_.push_back(bindingObject);
        if (type == "Buffer")
        {
            bindingObject->type = MaterailBindingObjectType::BUFFER;
            bindingObject->name = cfg["name"].GetString();
            bindingObject->binding = cfg["binding"].GetUint();
            bindingObject->stride = cfg["stride"].GetUint();

            std::vector<MaterialParameter> fields;

            if (cfg.HasMember("fields") && cfg["fields"].IsArray())
            {
                for (const auto& f : cfg["fields"].GetArray())
                {
                    MaterialParameter param;
                    param.name = f["name"].GetString();
                    param.offset = f["offset"].GetUint();
                    std::string format = f["format"].GetString();
                    switch (_SimpleHash(format.c_str()))
                    {
                    case _SimpleHash("mat4"):
                        param.format = MaterialParameterType::MAT4;
                        break;
                    case _SimpleHash("float"):
                        param.format = MaterialParameterType::FLOAT;
                        break;
                    case _SimpleHash("float2"):
                        param.format = MaterialParameterType::FLOAT2;
                        break;
                    case _SimpleHash("float3"):
                        param.format = MaterialParameterType::FLOAT3;
                        break;
                    case _SimpleHash("float4"):
                        param.format = MaterialParameterType::FLOAT4;
                        break;
                    default:
                        Assert(false, "invalid format");
                    }

                    fields.push_back(param);
                }

                //Assert(bindingObject->stride > 0, "Binding buffer's size must be great than zero");

                RHI::BufferDescriptor bufferDescriptor;
                {
                    bufferDescriptor.size = bindingObject->stride;
                    bufferDescriptor.usage = RHI::BufferUsage::UNIFORM;
                };
                bindingObject->buffer = Engine::GetGPUDevice()->CreateBuffer(bufferDescriptor);
                RHI_SAFE_RETAIN(bindingObject->buffer);

                for (auto& field : fields)
                {
                    field.bindingObject = bindingObject;
                    parameters_[field.name] = field;
                }
            }
        }
        else if (type == "Texture2D")
        {
            bindingObject->type = MaterailBindingObjectType::TEXTURE2D;
            bindingObject->name = cfg["name"].GetString();
            bindingObject->binding = cfg["binding"].GetUint();
            std::string uri = "Textures/default-512.png";
            if (cfg.HasMember("uri") && cfg["uri"].IsString() && cfg["uri"].GetStringLength())
            {
                uri = cfg["uri"].GetString();
            }
            bindingObject->texture = ImageLoader::LoadTextureFromUri(uri);
            RHI_SAFE_RETAIN(bindingObject->texture);

            MaterialParameter param;
            param.name = bindingObject->name;
            param.offset = 0;
            param.format = MaterialParameterType::TEXTURE2D;
            param.bindingObject = bindingObject;
            param.binding = bindingObject->binding;
            parameters_[param.name] = param;
        }
        else if (type == "Sampler2D")
        {
            bindingObject->type = MaterailBindingObjectType::SAMPLER2D;
            bindingObject->name = cfg["name"].GetString();
            bindingObject->binding = cfg["binding"].GetUint();

            RHI::SamplerDescriptor descriptor;
            {
                descriptor.minFilter = RHI::FilterMode::LINEAR;
                descriptor.magFilter = RHI::FilterMode::LINEAR;
                descriptor.addressModeU = RHI::AddressMode::REPEAT;
                descriptor.addressModeV = RHI::AddressMode::REPEAT;
                descriptor.addressModeW = RHI::AddressMode::REPEAT;
            }
            bindingObject->sampler = Engine::GetGPUDevice()->CreateSampler(descriptor);
            RHI_SAFE_RETAIN(bindingObject->sampler);
        }
        else Assert(false, "");

    }

    std::sort(bindingObjects_.begin(), bindingObjects_.end(),
        [](MaterialBindingObject* a, MaterialBindingObject* b)
        {
            return a->binding < b->binding;
        }
    );

    // TODO

    RHI::BindGroupLayoutDescriptor descriptor;
    for (const auto& bo : bindingObjects_)
    {
        RHI::BindGroupLayoutBinding target;
        target.binding = bo->binding;
        target.visibility = RHI::ShaderStage::VERTEX | RHI::ShaderStage::FRAGMENT;
        switch (bo->type)
        {
        case MaterailBindingObjectType::BUFFER:
            target.type = RHI::BindingType::UNIFORM_BUFFER;
            break;
        case MaterailBindingObjectType::TEXTURE2D:
            target.type = RHI::BindingType::SAMPLED_TEXTURE;
            break;
        case MaterailBindingObjectType::SAMPLER2D:
            target.type = RHI::BindingType::SAMPLER;
            break;
        }
        descriptor.entries.push_back(target);
    }
    rhiBindGroupLayout_ = Engine::GetGPUDevice()->CreateBindGroupLayout(descriptor);

    {
        RHI::PipelineLayoutDescriptor desc;
        {
            desc.bindGroupLayouts.push_back(rhiBindGroupLayout_);
        }
        rhiPipelineLayout_ = Engine::GetGPUDevice()->CreatePipelineLayout(desc);
    }

}

RHI::RenderPipeline* Material::CreatePipelineState(const PSOKey& psoKey, const ShaderSet& shaderSet)
{
    RHI::RenderPipelineDescriptor psoDesc;
    {
        psoDesc.layout = rhiPipelineLayout_;
        psoDesc.primitiveTopology = RHI::PrimitiveTopology::TRIANGLE_LIST;

        {
            psoDesc.vertexStage.shader = shaderSet.VertexShader;
            psoDesc.vertexStage.entryPoint = "main";
            psoDesc.fragmentStage.shader = shaderSet.FragmentShader;
            psoDesc.fragmentStage.entryPoint = "main";
        }

        // TODO read from json
        {
            psoDesc.depthStencilState = RHI::DepthStencilStateDescriptor();
            psoDesc.depthStencilState.format = RHI::TextureFormat::DEPTH24PLUS_STENCIL8;
            psoDesc.depthStencilState.depthWriteEnabled = psoKey.DepthWrite;
            psoDesc.depthStencilState.depthCompare = (RHI::CompareFunction)psoKey.DepthCmpFunc;
            psoDesc.depthStencilState.stencilFront = {};
            psoDesc.depthStencilState.stencilBack = {};
        }

        Assert(inputAssembler_.IsValid(), "invalid InputAssembler");
        psoDesc.vertexState = inputAssembler_.ToRHIDescriptor();

        psoDesc.rasterizationState = {
            .frontFace = (RHI::FrontFace)psoKey.FrontFace,
            .cullMode = (RHI::CullMode)psoKey.CullMode,
        };

        for (int i = 0; i < kMaxAttachmentCount; ++i)
        {
            if (psoKey.AttachmentFormats[i] != 0)
            {
                RHI::ColorStateDescriptor csd;
                {
                    csd.format = (RHI::TextureFormat)psoKey.AttachmentFormats[i];
                    csd.alphaBlend = {};
                    csd.colorBlend = {};
                    csd.writeMask = RHI::ColorWrite::ALL;
                }
                psoDesc.colorStates.push_back(csd);
            }
        }

        psoDesc.sampleCount = 1;
        psoDesc.sampleMask = 0xFFFFFFFF;
        psoDesc.alphaToCoverageEnabled = false;
    }

    return Engine::GetGPUDevice()->CreateRenderPipeline(psoDesc);
}

void Material::BindPSO(RHI::RenderPassEncoder& passEndcoder)
{
}

ShaderSet Material::CreateShaderSet(ETechniqueType technique)
{
    std::vector<std::string> userDefines;

    switch (technique)
    {
    case ETechniqueType::TGBufferGen:
        userDefines.push_back("#define GEN_GBUFFER_PASS 1");
        break;
    }

    uint32 iaLocation = 0;
    userDefines.push_back("#define IA_LOCATION_POSITION " + std::to_string(iaLocation++));

    for (const InputAttribute& ia : inputAssembler_.inputAttributes_)
    {
        switch (ia.kind)
        {
        case VertexBufferAttriKind::NORMAL:
            userDefines.push_back("#define USE_NORMAL 1");
            userDefines.push_back("#define IA_LOCATION_NORMAL " + std::to_string(iaLocation++));
            break;
        case VertexBufferAttriKind::DIFFUSE:
            userDefines.push_back("#define USE_DIFFUSE 1");
            userDefines.push_back("#define IA_LOCATION_DIFFUSE " + std::to_string(iaLocation++));
            break;
        case VertexBufferAttriKind::TEXCOORD:
            userDefines.push_back("#define USE_UV0 1");
            userDefines.push_back("#define IA_LOCATION_TEXCOORD " + std::to_string(iaLocation++));
            break;
        case VertexBufferAttriKind::TANGENT:
            userDefines.push_back("#define USE_TANGENT 1");
            userDefines.push_back("#define IA_LOCATION_TANGENT " + std::to_string(iaLocation++));
            break;
        case VertexBufferAttriKind::BINORMAL:
            userDefines.push_back("#define USE_BINORMAL 1");
            userDefines.push_back("#define IA_LOCATION_BINORMAL " + std::to_string(iaLocation++));
            break;
        case VertexBufferAttriKind::BITANGENT:
            userDefines.push_back("#define USE_BITANGENT 1");
            userDefines.push_back("#define IA_LOCATION_BITANGENT " + std::to_string(iaLocation++));
            break;
        case VertexBufferAttriKind::TEXCOORD2:
            userDefines.push_back("#define USE_UV0 1");
            userDefines.push_back("#define IA_LOCATION_TEXCOORD2 " + std::to_string(iaLocation++));
            break;
        }
    }

    ShaderSet shaderSet;

    if (vsFilename_.size())
    {
        shaderSet.VertexShader = _CreateShader(vsFilename_, RHI::ShaderType::VERTEX, technique, userDefines);
    }

    if (fsFilename_.size())
    {
        shaderSet.FragmentShader = _CreateShader(fsFilename_, RHI::ShaderType::FRAGMENT, technique, userDefines);
    }

    return shaderSet;
}

NS_RX_END