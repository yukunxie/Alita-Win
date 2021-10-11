//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MATERIAL_H
#define ALITA_MATERIAL_H

#include "Base/ObjectBase.h"
#include "Base/FileSystem.h"
#include "Effect.h"
#include "GFX/GFX.h"
#include "Pass.h"
#include "GFX/Backend/Vulkan/ShaderHelper.h"
#include "Graphics/Texture.h"

#define RAPIDJSON_ASSERT(x) Assert(x, "")

#include "rapidjson/document.h"

#include <list>
#include <map>
#include <array>

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

struct TextureSamplerBindingObject
{
	const gfx::Texture* texture = nullptr;
	const gfx::Sampler* sampler = nullptr;
	std::string			preprocessor  = "";
};

struct BufferBindingObject
{
	std::uint32_t stride = 0;
	const gfx::Buffer* buffer = nullptr;
};

struct MaterialBindingObject
{
	MaterailBindingObjectType	type;
	std::uint32_t				binding;
	std::string					name;
	BufferBindingObject			Buffer;
	TextureSamplerBindingObject Texture;
	
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

	gfx::VertexFormat ToRHIFormat() const
	{
		switch (format)
		{
		case rx::InputAttributeFormat::FLOAT:
			return gfx::VertexFormat::FLOAT;
		case rx::InputAttributeFormat::FLOAT2:
			return gfx::VertexFormat::FLOAT2;
		case rx::InputAttributeFormat::FLOAT3:
			return gfx::VertexFormat::FLOAT3;
		case rx::InputAttributeFormat::FLOAT4:
			return gfx::VertexFormat::FLOAT4;
		}
		Assert(false, "invalid format");
		return gfx::VertexFormat::FLOAT;
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
	}

	gfx::VertexStateDescriptor ToRHIDescriptor()
	{
		gfx::VertexStateDescriptor ret;
		ret.indexFormat = indexType_ == IndexType::UINT32? gfx::IndexFormat::UINT32 : gfx::IndexFormat::UINT16;
		for (const auto& ia : inputAttributes_)
		{
			gfx::VertexBufferDescriptor vbDesc;
			vbDesc.stepMode = gfx::InputStepMode::VERTEX;
			vbDesc.arrayStride = ia.stride;

			gfx::VertexAttributeDescriptor vaDesc;
			vaDesc.format = ia.ToRHIFormat();
			vaDesc.offset = ia.offset;
			vaDesc.shaderLocation = ia.location;

			vbDesc.attributes.push_back(vaDesc);

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

struct ShadingStateHasher
{
	gfx::Format attachments[kMaxAttachmentCount];
	gfx::FrontFace frontFace : 1;
	gfx::CullMode cullMode : 2; 
};

struct PSOKey
{
	PSOKey() noexcept
	{
		memset(this, 0, sizeof(*this));
		this->Technique = (int)ETechniqueType::TShading;
		this->DepthBias = 0;
		this->DepthWrite = 1;
		this->DepthCmpFunc = (int)gfx::CompareFunction::LESS_EQUAL;
		this->StencilWrite = 0;
		this->StencilCmpFunc = (int)gfx::CompareFunction::NEVER;
		this->StencilFailOp = (int)gfx::StencilOperation::KEEP;
		this->StencilDepthFailOp = (int)gfx::StencilOperation::KEEP;
		this->StencilPassOp = (int)gfx::StencilOperation::KEEP;
		this->StencilMask = 0;
		this->FrontFace = (int)gfx::FrontFace::COUNTER_CLOCKWISE;
		this->CullMode = (int)gfx::CullMode::BACK_BIT;
	}

	PSOKey(const PSOKey& lhr) noexcept
	{
		memcpy(this, &lhr, sizeof(*this));
	}

	inline bool operator==(const PSOKey& lhr) noexcept
	{
		return 0 == memcmp(this, &lhr, sizeof(lhr));
	}
	
	uint8 Technique : 8;
	uint8 DepthBias : 1;
	uint8 DepthWrite : 1;
	uint8 DepthCmpFunc : 3;
	uint8 StencilWrite : 1;
	uint8 StencilCmpFunc : 3;
	uint8 StencilFailOp : 3;
	uint8 StencilDepthFailOp : 3;
	uint8 StencilPassOp : 3;
	uint8 FrontFace : 1;
	uint8 CullMode : 2;
	uint8 StencilMask : 8;
	uint8 AttachmentFormats[kMaxAttachmentCount];
	uint8 DSAttachmentFormat: 8;
};

NS_RX_END

namespace std
{
	template<>
	struct hash<rx::PSOKey>
	{
		std::size_t operator()(const rx::PSOKey& s) const
		{
			const unsigned _FNV_offset_basis = 2166136261U;
			const unsigned _FNV_prime = 16777619U;
			unsigned _Val = _FNV_offset_basis;
			size_t _Count = sizeof(rx::PSOKey);
			const char* _First = (const char*)&s;
			for (size_t _Next = 0; _Next < _Count; ++_Next)
			{
				_Val ^= (unsigned)_First[_Next];
				_Val *= _FNV_prime;
			}

			return _Val;
		}
	};

	inline bool operator==(const rx::PSOKey& lhs, const rx::PSOKey& lhr) noexcept
	{
		return 0 == memcmp(&lhs, &lhr, sizeof(lhs));
	}
}

NS_RX_BEGIN

struct ShaderSet
{
	gfx::Shader* VertexShader = nullptr;
	gfx::Shader* FragmentShader = nullptr;
	gfx::Shader* ComputeShader = nullptr;
};

struct TechniqueShaderEntry
{
	bool Declared = false;
	std::string VertexShaderEntry;
	std::string FragmentShaderEntry;
};

class Material : public ObjectBase
{
public:
	Material(const std::string& configFilename = "");

	virtual void Build() {}

	bool SetFloat(const std::string& name, std::uint32_t offset, std::uint32_t count, const float* data);

	bool SetTexture(const std::string& name, const gfx::Texture* texture);

	bool SetTexture(const std::string& name, std::shared_ptr<Texture>& texture);

	void Apply(const Pass* pass, ETechniqueType technique, ERenderSet renderSet, gfx::RenderPassEncoder& passEndcoder);

	const std::vector<InputAttribute>& GetInputAttributes() const
	{
		return inputAttributes_;
	}

	void SetInputAssembler(const InputAssembler& IA)
	{
		inputAssembler_ = IA;
		GFX_SAFE_RELEASE(rhiPipelineState_);
	}

protected:
	gfx::Shader* _CreateShader(const std::string& filename, gfx::ShaderType shaderType, ETechniqueType techType, const std::vector<std::string>& userDefines);
	gfx::RenderPipeline* CreatePipelineState(const PSOKey& psoKey, const ShaderSet& shaderSet);
	gfx::SamplerDescriptor ParseSamplerDescriptor(const rapidjson::Value& doc);
	void ParseBindGroupLayout(const rapidjson::Document& doc);
	void SetupConstantBufferLayout();

	void PrepareBindingLayout();
	void ApplyModifyToBindGroup(gfx::RenderPassEncoder& passEndcoder);
	void BindPSO(gfx::RenderPassEncoder& passEndcoder);

	void SetupPSOKey(PSOKey& psoKey, ERenderSet renderSet);
	void SetupPSOKey(PSOKey& psoKey, ETechniqueType technique);
	void SetupPSOKey(PSOKey& psoKey, const Pass* pass);

	ShaderSet CreateShaderSet(ETechniqueType technique);
	

protected:
	std::string vsFilename_;
	std::string fsFilename_;

	Effect* effect_ = nullptr;
	InputAssembler inputAssembler_;

	std::map<std::string, MaterialParameter> parameters_;
	std::vector<MaterialBindingObject*> bindingObjects_;
	std::vector<InputAttribute> inputAttributes_;
	std::vector<std::string> SystemDefines_;

	// RHI
	gfx::BindGroupLayout* rhiBindGroupLayout_ = nullptr;
	gfx::PipelineLayout* rhiPipelineLayout_ = nullptr;
	gfx::BindGroup* rhiBindGroup_ = nullptr;
	gfx::Shader* rhiVertShader_ = nullptr;
	gfx::Shader* rhiFragShader_ = nullptr;
	gfx::RenderPipeline* rhiPipelineState_ = nullptr;

	std::array<TechniqueShaderEntry, (int)ETechniqueType::TMaxCount> TechniqueShaderEntries_;
	std::unordered_map< PSOKey, gfx::RenderPipeline*> rhiPSOMap_;

	std::map<std::string, std::shared_ptr<Resource>> BindingResources_;

	bool bBindingDirty_ = true;
};

NS_RX_END


#endif //ALITA_MATERIAL_H
