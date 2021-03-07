//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MATERIAL_H
#define ALITA_MATERIAL_H

#include "Base/ObjectBase.h"
#include "Base/FileSystem.h"
#include "Effect.h"
#include "RHI.h"
//#include "Meshes/VertexBuffer.h"

#include "rapidjson/document.h"

#include <list>
#include <map>

NS_RX_BEGIN

class Material;

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

std::uint32_t GetInputAttributeLocation(VertexBufferAttriKind kind);

std::uint32_t GetFormatSize(InputAttributeFormat format);


struct InputAttribute
{
	//std::string name;
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

	bool operator < (const InputAttribute& other)
	{
		return location < other.location;
	}
};

class InputAssembler
{
public:
	InputAssembler()
	{
	}

	InputAssembler(const InputAssembler& other)
	{
		hash_ = other.hash_;
		inputAttributes_ = other.inputAttributes_;
		indexType_ = other.indexType_;
	}

	InputAssembler(const std::vector<InputAttribute>& inputAttributes, IndexType indexType)
	{
		hash_ = 1;
		inputAttributes_ = inputAttributes;
		indexType_ = indexType;
		
		// sort by location
		std::sort(inputAttributes_.begin(), inputAttributes_.end());
	}

	RHI::VertexInputDescriptor ToRHIDescriptor()
	{
		RHI::VertexInputDescriptor ret;
		ret.indexFormat = indexType_ == IndexType::UINT32? RHI::IndexFormat::UINT32 : RHI::IndexFormat::UINT16;
		for (const auto& ia : inputAttributes_)
		{
			RHI::VertexBufferDescriptor vbDesc;
			vbDesc.stepMode = RHI::InputStepMode::VERTEX;
			vbDesc.stride = ia.stride;

			RHI::VertexAttributeDescriptor vaDesc;
			vaDesc.format = ia.ToRHIFormat();
			vaDesc.offset = ia.offset;
			vaDesc.shaderLocation = ia.location;

			vbDesc.attributeSet.push_back(vaDesc);

			ret.vertexBuffers.push_back(vbDesc);
		}
		return ret;
	}

	InputAssembler& operator= (const InputAssembler& other)
	{
		hash_ = other.hash_;
		inputAttributes_ = other.inputAttributes_;
		indexType_ = other.indexType_;
		return *this;
	}

	bool IsValid()
	{
		return hash_ != 0;
	}

protected:
	std::uint64_t hash_ = 0;
	std::vector<InputAttribute> inputAttributes_;
	IndexType indexType_;

	friend class Material;
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

	void SetInputAssembler(const InputAssembler& IA)
	{
		inputAssembler_ = IA;
		RHI_SAFE_RELEASE(rhiPipelineState_);
	}

protected:
	void CreatePipelineState();
	void ParseBindGroupLayout(const rapidjson::Document& doc);
	void ParseInputAssembler(const rapidjson::Document& doc);

	void ApplyModifyToBindGroup(RHI::RenderPassEncoder& passEndcoder);
	void BindPSO(RHI::RenderPassEncoder& passEndcoder);

protected:
	std::string vsFilename_;
	std::string fsFilename_;

	Effect* effect_ = nullptr;
	InputAssembler inputAssembler_;

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

	bool bBindingDirty_ = true;
};

NS_RX_END


#endif //ALITA_MATERIAL_H
