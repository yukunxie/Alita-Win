//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MATERIAL_H
#define ALITA_MATERIAL_H

#include "Base/ObjectBase.h"
#include "Base/FileSystem.h"
#include "Effect.h"
#include "RHI/RHI.h"
//#include "Meshes/VertexBuffer.h"

#define RAPIDJSON_ASSERT(x) Assert(x, "")

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

	RHI::VertexStateDescriptor ToRHIDescriptor()
	{
		RHI::VertexStateDescriptor ret;
		ret.indexFormat = indexType_ == IndexType::UINT32? RHI::IndexFormat::UINT32 : RHI::IndexFormat::UINT16;
		for (const auto& ia : inputAttributes_)
		{
			RHI::VertexBufferDescriptor vbDesc;
			vbDesc.stepMode = RHI::InputStepMode::VERTEX;
			vbDesc.arrayStride = ia.stride;

			RHI::VertexAttributeDescriptor vaDesc;
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

constexpr std::uint32_t kMaxAttachmentCount = 6;
struct ShadingStateHasher
{
	RHI::Format attachments[kMaxAttachmentCount];
	RHI::FrontFace frontFace : 1;
	RHI::CullMode cullMode : 2; 
};

struct PSOKey
{
	PSOKey() noexcept
	{
		memset(this, 0, sizeof(*this));
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
	uint8 DepthWrite : 1;
	uint8 DepthCmpFunc : 3;
	uint8 StencilWrite : 1;
	uint8 StencilCmpFunc : 3;
	uint8 StencilFailOp : 3;
	uint8 StencilDepthFailOp : 3;
	uint8 StencilPassOp : 3;
	uint8 StencilMask : 8;
	uint8 FrontFace : 1;
	uint8 CullMode : 2;
	uint8 AttachmentFormat0 : 8;
	uint8 AttachmentFormat1 : 8;
	uint8 AttachmentFormat2 : 8;
	uint8 AttachmentFormat3 : 8;
	uint8 AttachmentFormat4 : 8;
	uint8 AttachmentFormat5 : 8;
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

	std::unordered_map< PSOKey, RHI::RenderPipeline*> rhiPSOMap_;

	RHI::RenderPipeline* rhiPipelineStateObjects_[(uint32)ETechniqueType::TMaxCount] = { nullptr };

	bool bBindingDirty_ = true;
};

NS_RX_END


#endif //ALITA_MATERIAL_H
