//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MATERIAL_H
#define ALITA_MATERIAL_H

#include "Base/ObjectBase.h"
#include "Base/FileSystem.h"
#include "Effect.h"
#include "RHI.h"
#include "Meshes/VertexBuffer.h"

#include "rapidjson/document.h"

#include <list>
#include <map>

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

	BUFFER,
	SAMPLER2D,
	TEXTURE2D,
};

enum class InputAttributeFormat
{
	FLOAT,
	FLOAT2,
	FLOAT3,
	FLOAT4,
};

struct MaterialBindingObject;

struct MaterialParameter
{
	std::string name;
	MaterialParameterType format;
	union
	{
		std::uint32_t offset;
		std::uint32_t binding;
	};
	MaterialBindingObject* bindingObject = nullptr;
};

enum class MaterailBindingObjectType
{
	BUFFER,
	SAMPLER2D,
	TEXTURE2D,
};

struct MaterialBindingObject
{
	std::string name;
	std::uint32_t binding;
	std::uint32_t stride = 0;
	MaterailBindingObjectType type;
	union
	{
		RHI::Buffer* buffer;
		RHI::Texture* texture;
		RHI::Sampler* sampler;
	};
};

enum InputAttributeLocation
{
	IA_LOCATION_POSITION = 0,
	IA_LOCATION_NORMAL = 1,
	IA_LOCATION_TEXCOORD = 2,
	IA_LOCATION_DIFFUSE = 3,
	IA_LOCATION_TANGENT = 4,
	IA_LOCATION_BINORMAL = 5,
	IA_LOCATION_BITANGENT = 6,
	IA_LOCATION_TEXCOORD2 = 7,
};

struct InputAttribute
{
	std::string name;
	std::uint32_t location = 0;
	std::uint32_t offset = 0;
	std::uint32_t stride = 0;
	InputAttributeFormat format;
	VertexBufferAttriKind kind = VertexBufferAttriKind::INVALID;

	RHI::VertexFormat ToRHIFormat() const
	{
		switch (format)
		{
		case rx::InputAttributeFormat::FLOAT:
			return RHI::VertexFormat::FLOAT;
		case rx::InputAttributeFormat::FLOAT2:
			return RHI::VertexFormat::FLOAT2;
		case rx::InputAttributeFormat::FLOAT3:
			return RHI::VertexFormat::FLOAT3;
		case rx::InputAttributeFormat::FLOAT4:
			return RHI::VertexFormat::FLOAT4;
		}
		Assert(false, "invalid format");
		return RHI::VertexFormat::FLOAT;
	}
};

class Material : public ObjectBase
{
public:
	Material(const std::string& configFilename = "");

	virtual void Build() {}

	bool SetFloat(const std::string& name, std::uint32_t offset, std::uint32_t count, float* data);

	bool SetTexture(const std::string& name, RHI::Texture* texture);

	void Apply(RHI::RenderPassEncoder& passEndcoder);

	const std::vector<InputAttribute>& GetInputAttributes() const
	{
		return inputAttributes_;
	}

protected:
	void CreatePipelineState();
	void ParseBindGroupLayout(const rapidjson::Document& doc);
	void ParseInputAssembler(const rapidjson::Document& doc);

	void ApplyModifyToBindGroup();

protected:
	Effect* effect_ = nullptr;

	std::map<std::string, MaterialParameter> parameters_;
	std::vector<MaterialBindingObject*> bindingObjects_;
	std::vector<InputAttribute> inputAttributes_;

	// RHI
	RHI::BindGroupLayout* rhiBindGroupLayout_ = nullptr;
	RHI::PipelineLayout* rhiPipelineLayout_ = nullptr;
	RHI::BindGroup* rhiBindGroup_ = nullptr;
	RHI::Shader* rhiVertShader_ = nullptr;
	RHI::Shader* rhiFragShader_ = nullptr;
	RHI::RenderPipeline* rhiPipelineState_ = nullptr;

	bool bindingDirty_ = true;
};

NS_RX_END


#endif //ALITA_MATERIAL_H
