//
// Created by realxie on 2019-10-29.
//

#include "Material.h"
#include "Engine/Engine.h"
#include "Loaders/ImageLoader.h"

#include "RHI.h"
#include "Backend/Vulkan/ShaderHelper.h"

NS_RX_BEGIN

constexpr std::uint32_t _SimpleHash(const char* p)
{
	std::uint32_t hash = 0;
	for (; *p; p++)
	{
		hash = hash * 31 + *p;
	}
	return hash;
}

static RHI::ShaderModule* _CreateShader(const std::string& filename, RHI::ShaderType shaderType)
{
	std::string shaderText = FileSystem::GetInstance()->GetStringData(filename.c_str());

	auto spirV = RHI::CompileGLSLToSPIRV(shaderText, shaderType);
	RHI::ShaderModuleDescriptor descriptor;
	descriptor.binaryCode = std::move(spirV);
	descriptor.codeType = RHI::ShaderCodeType::BINARY;
	return Engine::GetGPUDevice()->CreateShaderModule(descriptor);
}

Material::Material(const std::string& configFilename)
{
	rapidjson::Document doc;
	{
		std::string text = FileSystem::GetInstance()->GetStringData("Shaders/CommonMaterial.json");
		doc.Parse(text.c_str());
	}

	rhiVertShader_ = _CreateShader(doc["code"]["vs"].GetString(), RHI::ShaderType::VERTEX);
	rhiFragShader_ = _CreateShader(doc["code"]["fs"].GetString(), RHI::ShaderType::FRAGMENT);

	ParseBindGroupLayout(doc);
	ParseInputAssembler(doc);
	CreatePipelineState();
}

void Material::Apply(RHI::RenderPassEncoder& passEndcoder)
{
	passEndcoder.SetGraphicPipeline(rhiPipelineState_);
	passEndcoder.SetBindGroup(0, rhiBindGroup_);
}

bool Material::SetFloat(const std::string& name, std::uint32_t offset, std::uint32_t count, float* data)
{
	const auto it = parameters_.find(name);
	if (it == parameters_.end())
		return false;

	const MaterialParameter& param = it->second;
	Assert(param.bindingObject->type == MaterailBindingObjectType::BUFFER, "");
	Assert(param.offset + offset + size < param.bindingObject->stride);
	param.bindingObject->buffer->SetSubData(param.offset + offset, count * sizeof(float), data);

	return true;
}

void Material::ParseInputAssembler(const rapidjson::Document& doc)
{
	if (!doc.HasMember("attributes"))
	{
		return;
	}
	Assert(doc["attributes"].IsArray());

	for (auto& cfg : doc["attributes"].GetArray())
	{
		InputAttribute attri;

		attri.name = cfg["name"].GetString();
		attri.location = cfg["location"].GetUint();
		attri.offset = cfg.HasMember("offset") ? cfg["offest"].GetUint() : 0;
		attri.stride = cfg.HasMember("stride") ? cfg["stride"].GetUint() : 0;
		std::string format = cfg["format"].GetString();

		switch (_SimpleHash(format.c_str()))
		{
		case _SimpleHash("float"):
			attri.format = InputAttributeFormat::FLOAT;
			break;
		case _SimpleHash("float2"):
			attri.format = InputAttributeFormat::FLOAT2;
			break;
		case _SimpleHash("float3"):
			attri.format = InputAttributeFormat::FLOAT3;
			break;
		case _SimpleHash("float4"):
			attri.format = InputAttributeFormat::FLOAT4;
			break;
		default:
			Assert(false, "invalid format");
			break;
		}

		inputAttributes_.push_back(attri);
	}

	std::sort(inputAttributes_.begin(), inputAttributes_.end(),
		[](const InputAttribute& a, const InputAttribute& b)
		{
			return a.location < b.location;
		});
}

void Material::ParseBindGroupLayout(const rapidjson::Document& doc)
{
	if (!doc.HasMember("bindings"))
	{
		return;
	}
	Assert(doc["bindings"].IsArray());

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

				if (Assert(bindingObject->stride > 0), "Binding buffer's size must be great than zero");

				RHI::BufferDescriptor bufferDescriptor;
				{
					bufferDescriptor.size = bindingObject->stride;
					bufferDescriptor.usage = RHI::BufferUsage::UNIFORM;
				};
				bindingObject->buffer = Engine::GetGPUDevice()->CreateBuffer(bufferDescriptor);

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
			bindingObject->texture = ImageLoader::LoadTextureFromUri("");
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
		}
		else Assert(false);

	}

	std::sort(bindingObjects_.begin(), bindingObjects_.end(),
		[](MaterialBindingObject* a, MaterialBindingObject* b)
		{
			return a->binding < b->binding;
		}
	);

	// TODO
	{
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
				target.type = RHI::BindingType::TEXTURE;
				break;
			case MaterailBindingObjectType::SAMPLER2D:
				target.type = RHI::BindingType::SAMPLER;
				break;
			}
			descriptor.bindings.push_back(target);
		}
		rhiBindGroupLayout_ = Engine::GetGPUDevice()->CreateBindGroupLayout(descriptor);
	}

	RHI::BindGroupDescriptor descriptor;
	descriptor.layout = rhiBindGroupLayout_;

	for (auto it : bindingObjects_)
	{
		if (it->type == MaterailBindingObjectType::BUFFER)
		{
			auto bufferBinding = new RHI::BufferBinding(it->buffer, 0, it->stride);
			RHI::BindGroupBinding tmp;
			{
				tmp.binding = it->binding;
				tmp.resource = bufferBinding;
			}
			descriptor.bindings.push_back(tmp);
		}
		else if (it->type == MaterailBindingObjectType::TEXTURE2D)
		{
			RHI::BindGroupBinding tmp;
			{
				tmp.binding = it->binding;
				tmp.resource = it->texture->CreateView();
			}
			descriptor.bindings.push_back(tmp);
		}
		else if (it->type == MaterailBindingObjectType::SAMPLER2D)
		{
			RHI::BindGroupBinding tmp;
			{
				tmp.binding = it->binding;
				tmp.resource = it->sampler;
			}
			descriptor.bindings.push_back(tmp);
		}
		else
			Assert(false);
	}

	rhiBindGroup_ = Engine::GetGPUDevice()->CreateBindGroup(descriptor);

	return;
}

void Material::CreatePipelineState()
{
	// todo;
	{
		RHI::PipelineLayoutDescriptor desc;
		{
			desc.bindGroupLayouts.push_back(rhiBindGroupLayout_);
		}
		rhiPipelineLayout_ = Engine::GetGPUDevice()->CreatePipelineLayout(desc);
	}

	RHI::RenderPipelineDescriptor psoDesc;
	{
		psoDesc.layout = rhiPipelineLayout_;
		psoDesc.primitiveTopology = RHI::PrimitiveTopology::TRIANGLE_LIST;

		{
			psoDesc.vertexStage.shaderModule = rhiVertShader_;
			psoDesc.vertexStage.entryPoint = "main";
			psoDesc.fragmentStage.shaderModule = rhiFragShader_;
			psoDesc.fragmentStage.entryPoint = "main";
		}

		// TODO read from json
		{
			psoDesc.depthStencilState = RHI::DepthStencilStateDescriptor();
			psoDesc.depthStencilState->format = RHI::TextureFormat::DEPTH24PLUS_STENCIL8;
			psoDesc.depthStencilState->depthWriteEnabled = true;
			psoDesc.depthStencilState->depthCompare = RHI::CompareFunction::LESS;
			psoDesc.depthStencilState->stencilFront = {};
			psoDesc.depthStencilState->stencilBack = {};
		}

		{
			// todo read from json
			psoDesc.vertexInput.indexFormat = RHI::IndexFormat::UINT32;
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

				psoDesc.vertexInput.vertexBuffers.push_back(vbDesc);
			}
		}

		psoDesc.rasterizationState = {
			.frontFace = RHI::FrontFace::COUNTER_CLOCKWISE,
			.cullMode = RHI::CullMode::BACK_BIT,
		};

		psoDesc.colorStates = {
			RHI::ColorStateDescriptor{
				.format = RHI::TextureFormat::BGRA8UNORM,
				.alphaBlend = {},
				.colorBlend = {},
				.writeMask = RHI::ColorWrite::ALL,
			}
		};

		psoDesc.sampleCount = 1;
		psoDesc.sampleMask = 0xFFFFFFFF;
		psoDesc.alphaToCoverageEnabled = false;
	}

	rhiPipelineState_ = Engine::GetGPUDevice()->CreateRenderPipeline(psoDesc);
}

NS_RX_END