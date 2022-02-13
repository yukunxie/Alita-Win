//
// Created by realxie on 2019-10-29.
//

#include "Material.h"
#include "Engine/Engine.h"
#include "Loaders/ImageLoader.h"
#include "RenderScene.h"
#include "Utils/MD5/md5.h"

#include <sstream>

NS_RX_BEGIN

std::map<std::string, std::vector<uint32>> gSprivShaderCache;

static constexpr std::uint32_t _SimpleHash(const char* p, uint32 key = 31)
{
    std::uint32_t hash = 0;
    for (; *p; p++)
    {
        hash = hash * key + *p;
    }
    return hash;
}

MaterialParameterType ToMaterialParameterType(const char* format)
{
    switch (_SimpleHash(format))
    {
    case _SimpleHash("float"): return MaterialParameterType::FLOAT;
    case _SimpleHash("float2"): return MaterialParameterType::FLOAT2;
    case _SimpleHash("float3"): return MaterialParameterType::FLOAT3;
    case _SimpleHash("float4"): return MaterialParameterType::FLOAT4;
    case _SimpleHash("int"): return MaterialParameterType::INT;
    case _SimpleHash("int2"): return MaterialParameterType::INT2;
    case _SimpleHash("int3"): return MaterialParameterType::INT3;
    case _SimpleHash("int4"): return MaterialParameterType::INT4;
    case _SimpleHash("bool"): return MaterialParameterType::BOOL;
    case _SimpleHash("bool1"): return MaterialParameterType::BOOL1;
    case _SimpleHash("bool2"): return MaterialParameterType::BOOL2;
    case _SimpleHash("bool3"): return MaterialParameterType::BOOL3;
    case _SimpleHash("mat4"): return MaterialParameterType::MAT4;
    case _SimpleHash("mat3"): return MaterialParameterType::MAT3;
    case _SimpleHash("mat2"): return MaterialParameterType::MAT2;
    case _SimpleHash("mat4x3"): return MaterialParameterType::MAT4x3;
    case _SimpleHash("mat4x2"): return MaterialParameterType::MAT4x2;
    case _SimpleHash("mat3x4"): return MaterialParameterType::MAT3x4;
    case _SimpleHash("mat2x4"): return MaterialParameterType::MAT2x4;
    case _SimpleHash("mat3x2"): return MaterialParameterType::MAT3x2;
    case _SimpleHash("Buffer"): return MaterialParameterType::BUFFER;
    case _SimpleHash("Sampler2D"): return MaterialParameterType::SAMPLER2D;
    case _SimpleHash("Texture2D"): return MaterialParameterType::TEXTURE2D;
    default:
        GFX_ASSERT(false, "invalid format");
    }
}

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
    GFX_ASSERT(false, "");
    return 0;
}

static const std::string sShaderExtensions =
"#version 450\n"
"#extension GL_ARB_separate_shader_objects : enable\n\n";

struct GLDataTypeConfig
{
    std::string Name;
    uint32      Size;
    uint32      ComponentSize;
    // std140 // https://blog.csdn.net/fatcat123/article/details/116144288
    uint32      Align;
};

static std::unordered_map<MaterialParameterType, GLDataTypeConfig> sShaderDataTypeToGLType
{
    {MaterialParameterType::FLOAT,      {"float",   4,   4,   4   }},
    {MaterialParameterType::FLOAT2,     {"vec2",    8,   4,   8   }},
    {MaterialParameterType::FLOAT3,     {"vec3",    12,  4,   16  }},
    {MaterialParameterType::FLOAT4,     {"vec4",    16,  4,   16  }},
    {MaterialParameterType::INT,        {"int",     4,   4,   4   }},
    {MaterialParameterType::INT2,       {"int2",    8,   4,   8   }},
    {MaterialParameterType::INT3,       {"int3",    12,  4,   16  }},
    {MaterialParameterType::INT4,       {"int4",    16,  4,   16  }},
    {MaterialParameterType::MAT2,       {"mat2",    16,  4,   16  }},
    {MaterialParameterType::MAT3,       {"mat3",    48,  4,   16  }},
    {MaterialParameterType::MAT4,       {"mat4",    64,  4,   16  }},
};

typedef std::vector<std::tuple<MaterialParameterType, std::string>> ConstantBufferFieldArray;

static ConstantBufferFieldArray sGlobalUnfiormFields
{
    {MaterialParameterType::FLOAT4,     "EyePos"},
    {MaterialParameterType::FLOAT4,     "SunLight"},
    {MaterialParameterType::FLOAT4,     "SunLightColor"},
    {MaterialParameterType::FLOAT4,     "ScreenInfo"},
    {MaterialParameterType::MAT4,       "ViewMatrix"},
    {MaterialParameterType::MAT4,       "ProjMatrix"},
    {MaterialParameterType::MAT4,       "ViewProjMatrix"},
    {MaterialParameterType::MAT4,       "ViewProjMatrixInverse"},
    {MaterialParameterType::MAT4,       "ShadowViewProjMatrix"},
};

static const std::string sShaderGlobalConstantBuffer =
"layout(binding = 0) uniform Global	                            \n"
"{	                                                            \n"
"    vec4 EyePos; // camera's world position	                \n"
"    vec4 SunLight; // xyz direction	                        \n"
"    vec4 SunLightColor; // rgb as color, and a as intensity    \n"
"    mat4 ViewMatrix;	                                        \n"
"    mat4 ProjMatrix;	                                        \n"
"} uGlobal;                                                     \n";


std::string GenerateConstantBufferDeclaration(const ConstantBufferFieldArray& fileds, uint32 binding, const std::string& typeName, const std::string& cBufferName)
{
    static const char* format =
        "layout(binding = %d) uniform %s\n"
        "{	                            \n"
        "%s                             \n"
        "} %s;                          \n";
    
    std::ostringstream ostr;
    for (const auto& field : fileds)
    {
        ostr << "\t" << sShaderDataTypeToGLType[std::get<0>(field)].Name << " \t" << std::get<1>(field) << ";" << "\t\n";
    }

    int size_s = std::snprintf(nullptr, 0, format, binding, typeName.c_str(), ostr.str().c_str(), cBufferName.c_str()) + 1; // Extra space for '\0'
    if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }

    auto size = static_cast<size_t>(size_s);
    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, format, binding, typeName.c_str(), ostr.str().c_str(), cBufferName.c_str());
    return std::string(buf.get(), buf.get() + size - 1);
}

void Material::SetupConstantBufferLayout()
{
    uint32 binding = 0;
    std::vector< MaterialParameter> parameters;
    std::string cBufferName = "uGlobal";

    uint32 offset = 0;

    for (const auto& field : sGlobalUnfiormFields)
    {
        parameters.emplace_back();
        auto& param = parameters.back();
        param.binding = binding;
        param.format  = std::get<0>(field);
        param.name    = std::get<1>(field);

        const uint32 byteAlign = sShaderDataTypeToGLType[param.format].Align;
        GFX_ASSERT((byteAlign & (byteAlign - 1)) == 0);
        offset = (offset + byteAlign - 1) & (~(byteAlign - 1));
        param.offset = offset;
        offset += sShaderDataTypeToGLType[param.format].Size;
    }

    MaterialBindingObject* bindingObject = new MaterialBindingObject();
    bindingObjects_.push_back(bindingObject);

    bindingObject->type     = MaterailBindingObjectType::BUFFER;
    bindingObject->name     = cBufferName;
    bindingObject->binding  = binding;
    bindingObject->Buffer.stride   = offset;

    gfx::BufferDescriptor bufferDescriptor;
    {
        bufferDescriptor.size = bindingObject->Buffer.stride;
        bufferDescriptor.usage = gfx::BufferUsage::UNIFORM;
    };
    bindingObject->Buffer.buffer = Engine::GetGPUDevice()->CreateBuffer(bufferDescriptor);

    for (auto& param : parameters)
    {
        param.bindingObject = bindingObject;
        parameters_[param.name] = param;
    }
}

gfx::ShaderPtr Material::_CreateShader(const std::string& filename, gfx::ShaderType shaderType, ETechniqueType techType, const std::vector<std::string>& userDefines)
{
    std::string data = FileSystem::GetInstance()->GetStringData(filename.c_str());

    std::string defines = "";
    for (const auto& d : userDefines)
    {
        defines += d + "\n";
    }

    const auto globalCBuffer = GenerateConstantBufferDeclaration(sGlobalUnfiormFields, 0, "Global", "uGlobal");

    std::string shaderText = sShaderExtensions
        + defines + "\n"
        + globalCBuffer + "\n"
        + data;

    GFX_ASSERT(TechniqueShaderEntries_[(int)techType].Declared);
    std::string techEntryName = "";
    switch (shaderType)
    {
    case  gfx::ShaderType::VERTEX:
        techEntryName = TechniqueShaderEntries_[(int)techType].VertexShaderEntry;
        break;
    case  gfx::ShaderType::FRAGMENT:
        techEntryName = TechniqueShaderEntries_[(int)techType].FragmentShaderEntry;
        break;
    default:
        GFX_ASSERT(false);
    }

    techEntryName = "void " + techEntryName + "()";  

    auto npos = shaderText.find(techEntryName.c_str(), 0);
    GFX_ASSERT(npos != std::string::npos);
    if (npos != std::string::npos)
    {
        shaderText = shaderText.replace(npos, techEntryName.size(), "void main()");
    }

    std::string hash = md5::md5(" shadertype=" + std::to_string((uint32)shaderType) + "\n" + shaderText);
    if (!gSprivShaderCache.contains(hash))
    {
        gSprivShaderCache[hash] = std::move(gfx::CompileGLSLToSPIRV(shaderText, shaderType));
    }

    const std::vector<uint32>& spirV = gSprivShaderCache[hash];
    
    gfx::ShaderModuleDescriptor descriptor;
    {
        descriptor.code.resize(spirV.size() * sizeof(spirV[0]));
        memcpy(descriptor.code.data(), spirV.data(), descriptor.code.size());
        descriptor.codeType = gfx::ShaderCodeType::BINARY;
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

    for (auto it = doc["techniques"].MemberBegin(); it != doc["techniques"].MemberEnd(); it++)
    {
       auto techName =  it->name.GetString();

       ETechniqueType technique = ETechniqueType::TShading;

       switch (_SimpleHash(techName))
       {
       case _SimpleHash("TShading"):
           technique = ETechniqueType::TShading;
           break;
       case _SimpleHash("TGBufferGen"):
           technique = ETechniqueType::TGBufferGen;
           break;
       case _SimpleHash("TShadowMapGen"):
           technique = ETechniqueType::TShadowMapGen;
           break;
       case _SimpleHash("TSkyBox"):
           technique = ETechniqueType::TSkyBox;
           break;
       case _SimpleHash("TOutline"):
           technique = ETechniqueType::TOutline;
           break;
       case _SimpleHash("TVolumeCloud"):
           technique = ETechniqueType::TVolumeCloud;
           break;
       default:
           GFX_ASSERT(false);
       }

       TechniqueShaderEntries_[(int)technique].Declared = true;
       TechniqueShaderEntries_[(int)technique].VertexShaderEntry = it->value["vs_entry"].GetString();
       TechniqueShaderEntries_[(int)technique].FragmentShaderEntry = it->value["fs_entry"].GetString();
    }

    ParseBindGroupLayout(doc);
}

void Material::Apply(const Pass* pass, ETechniqueType technique, ERenderSet renderSet, gfx::RenderPassEncoder& passEndcoder)
{
    // update binding buffer.
    {
        const GobalRenderParams& params = Engine::GetRenderScene()->GetGobalRenderParams();
        SetFloat("EyePos", 0, 3, (float*)&params.cameraWorldPosition.x);
        SetFloat("SunLight", 0, 3, (float*)&params.sunLightDirection.x);
        SetFloat("SunLightColor", 0, 4, (float*)&params.sunLightColor.x);
        SetFloat("ViewMatrix", 0, 16, (float*)&params.viewMatrix);
        SetFloat("ProjMatrix", 0, 16, (float*)&params.projMatrix);
        SetFloat("ViewProjMatrix", 0, 16, (float*)&params.viewProjMatrix);
        SetFloat("ShadowViewProjMatrix", 0, 16, (float*)&params.shadowViewProjMatrix); 

        auto viewProjMatrixInverse = glm::inverse(params.viewProjMatrix);
        SetFloat("ViewProjMatrixInverse", 0, 16, (float*)&viewProjMatrixInverse);

        auto wsize = Engine::GetRenderScene()->GetGraphicPipeline()->GetWindowSize();
        float screenInfo[4] = { wsize.width, wsize.height, 1.0f / wsize.width, 1.0f / wsize.height };
        SetFloat("ScreenInfo", 0, 4, screenInfo);
    }

    PSOKey psoKey{};
    SetupPSOKey(psoKey, technique);
    SetupPSOKey(psoKey, renderSet);
    SetupPSOKey(psoKey, pass);

    gfx::RenderPipelinePtr pso = nullptr;

    if (rhiPSOMap_.contains(psoKey))
    {
        pso = rhiPSOMap_[psoKey];
    }
    else
    {
        PrepareBindingLayout();
        ShaderSet shaders = CreateShaderSet(technique);
        pso = CreatePipelineState(psoKey, shaders);
        rhiPSOMap_[psoKey] = pso;
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
        psoKey.DepthCmpFunc = (int)gfx::CompareFunction::ALWAYS;
        break;
    default:
        break;
    }
}

void Material::SetupPSOKey(PSOKey& psoKey, ETechniqueType technique)
{
    psoKey.Technique = (int)technique;

    switch (technique)
    {
    case ETechniqueType::TShading:
        break;
    case ETechniqueType::TGBufferGen:
        psoKey.CullMode = gfx::CullMode::BACK_BIT;
        break;
    case ETechniqueType::TShadowMapGen:
        psoKey.CullMode = gfx::CullMode::BACK_BIT;
        psoKey.DepthBias = 1;
        break;
    case ETechniqueType::TSkyBox:
        psoKey.CullMode = gfx::CullMode::NONE;
        psoKey.DepthWrite = false;
        psoKey.StencilWrite = false;
        break;
    case ETechniqueType::TVolumeCloud:
        psoKey.CullMode = gfx::CullMode::NONE; //gfx::CullMode::FRONT_BIT;
        psoKey.DepthWrite = false;
        psoKey.StencilWrite = false;
        psoKey.ColorDstFactor = (uint8)gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        psoKey.ColorSrcFactor = (uint8)gfx::BlendFactor::SRC_ALPHA;
        psoKey.ColorOperation = (uint8)gfx::BlendOp::ADD;
        psoKey.AlphaDstFactor = (uint8)gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        psoKey.AlphaSrcFactor = (uint8)gfx::BlendFactor::SRC_ALPHA;
        psoKey.AlphaOperation = (uint8)gfx::BlendOp::ADD;
        break;
    default:
        break;
    }
}

void Material::SetupPSOKey(PSOKey& psoKey, const Pass* pass)
{
    for (const auto& tp : pass->GetColorAttachments())
    {
        GFX_ASSERT(tp.Slot < kMaxAttachmentCount);
        psoKey.AttachmentFormats[tp.Slot] = (uint8)tp.RenderTarget->GetFormat();
    }
    if (auto ds = pass->GetDSAttachment(); ds)
    {
        psoKey.DSAttachmentFormat = (uint8)ds->GetFormat();
    }
}

bool Material::SetFloat(const std::string& name, std::uint32_t offset, std::uint32_t count, const float* data)
{
    const auto it = parameters_.find(name);
    if (it == parameters_.end())
        return false;

    const MaterialParameter& param = it->second;
    Assert(param.bindingObject->type == MaterailBindingObjectType::BUFFER, "");
    Assert(param.offset + offset + size < param.bindingObject->stride, "");
    param.bindingObject->Buffer.buffer->SetSubData(param.offset + offset, count * sizeof(float), data);

    return true;
}

bool Material::SetTexture(const std::string& name, gfx::TexturePtr texture)
{
    Assert(texture != nullptr, "null is invalid");
    const auto it = parameters_.find(name);
    if (it == parameters_.end())
        return false;
    const MaterialParameter& param = it->second;
    Assert(param.format == MaterialParameterType::TEXTURE2D, "invalid");

    if (param.bindingObject->Texture.texture == texture)
        return true;
    // binding layout changed.
    if (!param.bindingObject->Texture.texture)
    {
        rhiPSOMap_.clear();
    }
    param.bindingObject->Texture.texture = texture;
    bBindingDirty_ = true;
    return true;
}

bool Material::SetTexture(const std::string& name, std::shared_ptr<Texture>& texture)
{
    BindingResources_[name] = texture;
    return SetTexture(name, texture->GetTexture());
}

void Material::ApplyModifyToBindGroup(gfx::RenderPassEncoder& passEndcoder)
{
    if (!bBindingDirty_)
        return;

    bBindingDirty_ = false;
    GFX_SAFE_RELEASE(rhiBindGroup_);

    gfx::BindGroupDescriptor descriptor;
    descriptor.layout = rhiBindGroupLayout_;

    for (auto it : bindingObjects_)
    {
        if (it->type == MaterailBindingObjectType::BUFFER)
        {
            auto resource = Engine::GetGPUDevice()->CreateBufferBinding(it->Buffer.buffer, 0, it->Buffer.stride);
            gfx::BindGroupBinding tmp;
            {
                tmp.binding = it->binding;
                tmp.resource = resource;
            }
            descriptor.entries.push_back(tmp);
        }
        else if (it->type == MaterailBindingObjectType::TEXTURE2D)
        {
            if (it->Texture.texture != nullptr)
            {
                gfx::TextureViewDescriptor tvDescriptor;
                if (it->Texture.texture->GetArrayLayerCount() == 6)
                {
                    tvDescriptor.dimension = gfx::TextureViewDimension::DIM_CUBE;
                }

                auto tv = Engine::GetGPUDevice()->CreateTextureView(it->Texture.texture, tvDescriptor);
                gfx::BindGroupBinding tmp;
                {
                    tmp.binding = it->binding;
                    tmp.resource = Engine::GetGPUDevice()->CreateTextureViewBinding(tv);
                }
                descriptor.entries.push_back(tmp);

                // create sampler for this texture
                auto sampler = Engine::GetGPUDevice()->CreateSamplerBinding(it->Texture.sampler);
                descriptor.entries.push_back(gfx::BindGroupBinding{ it->binding + 1 , sampler });
            }
        }
        else
        {
            GFX_ASSERT(false);
        }
    }
    rhiBindGroup_ = Engine::GetGPUDevice()->CreateBindGroup(descriptor);
}

gfx::SamplerDescriptor Material::ParseSamplerDescriptor(const rapidjson::Value& doc)
{
    gfx::SamplerDescriptor descriptor;
    {
        descriptor.minFilter = gfx::FilterMode::LINEAR;
        descriptor.magFilter = gfx::FilterMode::LINEAR;
        descriptor.mipmapFilter = gfx::FilterMode::LINEAR;
        descriptor.addressModeU = gfx::AddressMode::REPEAT;
        descriptor.addressModeV = gfx::AddressMode::REPEAT;
        descriptor.addressModeW = gfx::AddressMode::REPEAT;
    }
    if (!doc.HasMember("filter"))
    {
        return descriptor;
    }

    auto ParseFilterMode = [](const char* text) {
        switch (_SimpleHash(text))
        {
        case _SimpleHash("linear"):
            return gfx::FilterMode::LINEAR;
        case _SimpleHash("nearest"):
        case _SimpleHash("point"):
            return gfx::FilterMode::NEAREST;
        default:
            return gfx::FilterMode::LINEAR;
        }
    };

    auto ParseAddressMode = [](const char* text) {
        switch (_SimpleHash(text))
        {
        case _SimpleHash("repeat"):
            return gfx::AddressMode::REPEAT;
        case _SimpleHash("clamp"):
            return gfx::AddressMode::CLAMP_TO_EDGE;
        case _SimpleHash("mirror"):
            return gfx::AddressMode::MIRROR_REPEAT;
        default:
            return gfx::AddressMode::REPEAT;
        }
    };

    const auto& cfg = doc["filter"];

    if (cfg.HasMember("minFilter"))
    {
        descriptor.minFilter = ParseFilterMode(cfg["minFilter"].GetString());
    }
    if (cfg.HasMember("magFilter"))
    {
        descriptor.minFilter = ParseFilterMode(cfg["magFilter"].GetString());
    }
    if (cfg.HasMember("mipmapFilter"))
    {
        descriptor.minFilter = ParseFilterMode(cfg["mipmapFilter"].GetString());
    }
    if (cfg.HasMember("addressModeU"))
    {
        descriptor.addressModeU = ParseAddressMode(cfg["addressModeU"].GetString());
    }
    if (cfg.HasMember("addressModeV"))
    {
        descriptor.addressModeV = ParseAddressMode(cfg["addressModeV"].GetString());
    }
    if (cfg.HasMember("addressModeW"))
    {
        descriptor.addressModeW = ParseAddressMode(cfg["addressModeW"].GetString());
    }
    return descriptor;
}

void Material::ParseBindGroupLayout(const rapidjson::Document& doc)
{
    if (!doc.HasMember("bindings"))
    {
        return;
    }

    // uGlobal;
    SetupConstantBufferLayout();

    for (auto& cfg : doc["bindings"].GetArray())
    {
        // skip binding == 0, for global.
        if (cfg["binding"].GetUint() == 0)
            continue;

        std::string type = cfg["type"].GetString();
        
        if (type == "Buffer")
        {
            MaterialBindingObject* bindingObject = new MaterialBindingObject();
            bindingObjects_.push_back(bindingObject);

            bindingObject->type = MaterailBindingObjectType::BUFFER;
            bindingObject->name = cfg["name"].GetString();
            bindingObject->binding = cfg["binding"].GetUint();
            bindingObject->Buffer.stride = cfg["stride"].GetUint();

            std::vector<MaterialParameter> fields;

            if (cfg.HasMember("fields") && cfg["fields"].IsArray())
            {
                for (const auto& f : cfg["fields"].GetArray())
                {
                    MaterialParameter param;
                    param.name = f["name"].GetString();
                    param.offset = f["offset"].GetUint();
                    std::string format = f["format"].GetString();
                    param.format = ToMaterialParameterType(f["format"].GetString());
                    fields.push_back(param);
                }

                gfx::BufferDescriptor bufferDescriptor;
                {
                    bufferDescriptor.size = bindingObject->Buffer.stride;
                    bufferDescriptor.usage = gfx::BufferUsage::UNIFORM;
                };
                bindingObject->Buffer.buffer = Engine::GetGPUDevice()->CreateBuffer(bufferDescriptor);

                for (auto& field : fields)
                {
                    field.bindingObject = bindingObject;
                    parameters_[field.name] = field;
                }
            }
        }
        else if (type == "Texture2D")
        {
            // texture
            uint32 binding = cfg["binding"].GetUint();
            const char* name = cfg["name"].GetString();
            
            MaterialBindingObject* bindingObject = new MaterialBindingObject();
            bindingObjects_.push_back(bindingObject);

            if (cfg.HasMember("preprocessor"))
            {
                bindingObject->Texture.preprocessor = cfg["preprocessor"].GetString();
            }

            // texture
            bindingObject->type = MaterailBindingObjectType::TEXTURE2D;
            bindingObject->name = name;
            bindingObject->binding = binding;
            bindingObject->Texture.texture = nullptr;
            if (cfg.HasMember("cubeName"))
            {
                std::string cubeName = cfg["cubeName"].GetString();
                if (!cubeName.empty())
                {
                    auto texture = Texture::LoadCubeTexture(cubeName);
                    bindingObject->Texture.texture = texture->GetTexture();
                    BindingResources_[name] = texture;
                }
            }
            else if (cfg.HasMember("uri") && cfg["uri"].IsString() && cfg["uri"].GetStringLength())
            {
                std::string uri = cfg["uri"].GetString();
                if (!uri.empty())
                {
                    auto texture = Texture::LoadTextureFromUri(uri);
                    bindingObject->Texture.texture = texture->GetTexture();
                    BindingResources_[name] = texture;
                }
            }
            
            MaterialParameter param;
            param.name = bindingObject->name;
            param.offset = 0;
            param.format = MaterialParameterType::TEXTURE2D;
            param.bindingObject = bindingObject;
            param.binding = bindingObject->binding;
            parameters_[param.name] = param;
            
            // sampler
            gfx::SamplerDescriptor descriptor = ParseSamplerDescriptor(cfg);
            bindingObject->Texture.sampler = Engine::GetGPUDevice()->CreateSampler(descriptor);
        }
        else
        {
            GFX_ASSERT(false, "");
        }

    }

    std::sort(bindingObjects_.begin(), bindingObjects_.end(),
        [](MaterialBindingObject* a, MaterialBindingObject* b)
        {
            return a->binding < b->binding;
        }
    );
}

void Material::PrepareBindingLayout()
{
    SystemDefines_.clear();
    gfx::BindGroupLayoutDescriptor descriptor;
    for (const auto& bo : bindingObjects_)
    {
        switch (bo->type)
        {
        case MaterailBindingObjectType::BUFFER:
        {
            gfx::BindGroupLayoutBinding target;
            target.binding = bo->binding;
            target.visibility = gfx::ShaderStage::VERTEX | gfx::ShaderStage::FRAGMENT;
            target.type = gfx::BindingType::UNIFORM_BUFFER;
            descriptor.entries.push_back(target);

            break;
        }

        case MaterailBindingObjectType::TEXTURE2D:
        {
            if (bo->Texture.texture)
            {
                // texture
                {
                    gfx::BindGroupLayoutBinding target;
                    target.binding = bo->binding;
                    target.visibility = gfx::ShaderStage::VERTEX | gfx::ShaderStage::FRAGMENT;
                    target.type = gfx::BindingType::SAMPLED_TEXTURE;
                    descriptor.entries.push_back(target);
                }

                // sampler
                {
                    gfx::BindGroupLayoutBinding target;
                    target.binding = bo->binding + 1;
                    target.visibility = gfx::ShaderStage::VERTEX | gfx::ShaderStage::FRAGMENT;
                    target.type = gfx::BindingType::SAMPLER;
                    descriptor.entries.push_back(target);
                }

                if (!bo->Texture.preprocessor.empty())
                {
                    SystemDefines_.push_back("#define " + bo->Texture.preprocessor + " 1 ");
                }
            }
            break;
        }

        default:
            GFX_ASSERT(false);
        }

    }
    rhiBindGroupLayout_ = Engine::GetGPUDevice()->CreateBindGroupLayout(descriptor);

    {
        gfx::PipelineLayoutDescriptor desc;
        {
            desc.bindGroupLayouts.push_back(rhiBindGroupLayout_);
        }
        rhiPipelineLayout_ = Engine::GetGPUDevice()->CreatePipelineLayout(desc);
    }
}

gfx::RenderPipelinePtr Material::CreatePipelineState(const PSOKey& psoKey, const ShaderSet& shaderSet)
{
    gfx::RenderPipelineDescriptor psoDesc;
    {
        psoDesc.layout = rhiPipelineLayout_;
        psoDesc.primitiveTopology = gfx::PrimitiveTopology::TRIANGLE_LIST;

        {
            psoDesc.vertexStage.shader = shaderSet.VertexShader;
            psoDesc.vertexStage.entryPoint = "main";
            psoDesc.fragmentStage.shader = shaderSet.FragmentShader;
            psoDesc.fragmentStage.entryPoint = "main";
        }

        // TODO read from json
        {
            psoDesc.depthStencilState = gfx::DepthStencilStateDescriptor();
            psoDesc.depthStencilState.format = gfx::TextureFormat::DEPTH24PLUS_STENCIL8;
            psoDesc.depthStencilState.depthWriteEnabled = psoKey.DepthWrite;
            psoDesc.depthStencilState.depthCompare = (gfx::CompareFunction)psoKey.DepthCmpFunc;
            psoDesc.depthStencilState.stencilFront = {};
            psoDesc.depthStencilState.stencilBack = {};
        }

        Assert(inputAssembler_.IsValid(), "invalid InputAssembler");
        psoDesc.vertexState = inputAssembler_.ToRHIDescriptor();

        psoDesc.rasterizationState = {
            .frontFace = (gfx::FrontFace)psoKey.FrontFace,
            .cullMode = (gfx::CullMode)psoKey.CullMode,
            .depthBias = psoKey.DepthBias
        };

        for (int i = 0; i < kMaxAttachmentCount; ++i)
        {
            if (psoKey.AttachmentFormats[i] != 0)
            {
                gfx::ColorStateDescriptor csd;
                {
                    csd.format = (gfx::TextureFormat)psoKey.AttachmentFormats[i];
                    csd.colorBlend.dstFactor = (gfx::BlendFactor)psoKey.ColorDstFactor;
                    csd.colorBlend.srcFactor = (gfx::BlendFactor)psoKey.ColorSrcFactor;
                    csd.colorBlend.operation = (gfx::BlendOp)psoKey.ColorOperation;
                    csd.alphaBlend.dstFactor = (gfx::BlendFactor)psoKey.AlphaDstFactor;
                    csd.alphaBlend.srcFactor = (gfx::BlendFactor)psoKey.AlphaSrcFactor;
                    csd.alphaBlend.operation = (gfx::BlendOp)psoKey.AlphaOperation;
                    csd.writeMask = gfx::ColorWrite::ALL;
                }
                psoDesc.colorStates.push_back(csd);
            }
        }

        psoDesc.hasDepthStencilState = psoKey.DSAttachmentFormat != 0;

        psoDesc.sampleCount = 1;
        psoDesc.sampleMask = 0xFFFFFFFF;
        psoDesc.alphaToCoverageEnabled = false;
    }

    return Engine::GetGPUDevice()->CreateRenderPipeline(psoDesc);
}

void Material::BindPSO(gfx::RenderPassEncoder& passEndcoder)
{
}

ShaderSet Material::CreateShaderSet(ETechniqueType technique)
{
    std::vector<std::string> userDefines = SystemDefines_;

    switch (technique)
    {
    case ETechniqueType::TGBufferGen:
        userDefines.push_back("#define GEN_GBUFFER_PASS 1");
        break;
    }

    uint32 iaLocation = 0;

    for (const InputAttribute& ia : inputAssembler_.inputAttributes_)
    {
        switch (ia.kind)
        {
        case VertexBufferAttriKind::POSITION:
            userDefines.push_back("#define IA_LOCATION_POSITION " + std::to_string(iaLocation++));
            break;
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
        shaderSet.VertexShader = _CreateShader(vsFilename_, gfx::ShaderType::VERTEX, technique, userDefines);
    }

    if (fsFilename_.size() && technique != ETechniqueType::TShadowMapGen)
    {
        shaderSet.FragmentShader = _CreateShader(fsFilename_, gfx::ShaderType::FRAGMENT, technique, userDefines);
    }

    return shaderSet;
}

NS_RX_END