//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MATERIAL_H
#define ALITA_MATERIAL_H

#include "Base/ObjectBase.h"
#include "Base/FileSystem.h"
#include "Effect.h"

#include "rapidjson/document.h"

#include <list>

NS_RX_BEGIN

enum class MaterialParameterType
{
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,

    INT,
    INT2,
    INT3,
    INT4,

    BOOL,
    BOOL1,
    BOOL2,
    BOOL3,

    MAT4,
    MAT3,
    MAT2,
    MAT4x3,
    MAT4x2,
    MAT3x4,
    MAT2x4,
    MAT3x2,
    MAT2x3, 
};

struct MaterialBufferParameter
{
    std::string name;
    MaterialParameterType format;
    std::uint32_t offset = 0;
    RHI::Buffer* gpuBuffer_ = nullptr;
};

enum class MaterailBindingObjectType
{
    BUFFER,
    SAMPLER2D,
    TEXTURE2D,
};

class Material : public ObjectBase
{
public:
    Material(const std::string& configFilename = "");

    virtual void Build() {}

protected:
    RHI::BindGroupLayout* CreateBindGroupLayout(const rapidjson::Document& doc);

protected:
    Effect* effect_ = nullptr;

    // RHI
    RHI::BindGroupLayout* rhiBindGroupLayout_ = nullptr;
};

NS_RX_END


#endif //ALITA_MATERIAL_H
