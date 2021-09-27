#include "GLTFLoader.h"
#include "ImageLoader.h"
#include "World/MeshComponent.h"
#include "Graphics/Material.h"
#include <glm/gtx/matrix_decompose.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"


NS_RX_BEGIN

namespace GLTFLoader
{
	static InputAttributeFormat _GLTFTypeToIAFormat(int type)
	{
		switch (type)
		{
		case TINYGLTF_TYPE_SCALAR:
			return InputAttributeFormat::FLOAT;
		case TINYGLTF_TYPE_VEC2:
			return InputAttributeFormat::FLOAT2;
		case TINYGLTF_TYPE_VEC3:
			return InputAttributeFormat::FLOAT3;
		case TINYGLTF_TYPE_VEC4:
			return InputAttributeFormat::FLOAT4;
		default:
			Assert(false, "invalid type");
		}
		return InputAttributeFormat::FLOAT;
	}

	RHI::Texture* _LoadTexture(const tinygltf::Image& tImage)
	{
		auto width = tImage.width;
		auto height = tImage.height;
		auto component = tImage.component;

		Assert(tImage.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, "");

		return ImageLoader::LoadTextureFromData(width, height, component, tImage.image.data(), tImage.image.size());
	}


	void _LoadPrimitives(Entity* model, tinygltf::Model& tModel, const tinygltf::Mesh& tMesh)
	{
		for (const tinygltf::Primitive& primitive : tMesh.primitives)
		{
			MeshComponent* mc = new MeshComponent();
			mc->Geometry_ = new Geometry;
			mc->Material_ = new Material("Materials/PBR.json");
			{
				const tinygltf::Material& tMaterial = tModel.materials[primitive.material];
				std::vector<float> tmp;
				{
					tmp.resize(tMaterial.emissiveFactor.size());
					for (int i = 0; i < tmp.size(); ++i)
					{
						auto v = tMaterial.emissiveFactor[i];
						tmp[i] = (float)v;
					}
					mc->Material_->SetFloat("emissiveFactor", 0, std::min(3, (int)tmp.size()), tmp.data());
				}

				const tinygltf::PbrMetallicRoughness& pbrMetallicRoughness = tMaterial.pbrMetallicRoughness;
				{
					pbrMetallicRoughness.baseColorFactor.size();
					for (int i = 0; i < tmp.size(); ++i)
					{
						auto v = pbrMetallicRoughness.baseColorFactor[i];
						tmp[i] = (float)v;
					}
					mc->Material_->SetFloat("baseColorFactor", 0, std::min(4, (int)tmp.size()), tmp.data());
				}
				{
					float metallicFactor = (float)pbrMetallicRoughness.metallicFactor;
					mc->Material_->SetFloat("metallicFactor", 0, 1, &metallicFactor);

					float roughnessFactor = (float)pbrMetallicRoughness.roughnessFactor;
					mc->Material_->SetFloat("roughnessFactor", 0, 1, &roughnessFactor);
				}
				if (pbrMetallicRoughness.baseColorTexture.index != -1)
				{
					auto tIdx = pbrMetallicRoughness.baseColorTexture.index;
					const tinygltf::Image& image = tModel.images[tModel.textures[tIdx].source];

					auto texture = _LoadTexture(image);
					if (texture)
					{
						mc->Material_->SetTexture("albedo", texture);
					}
					/*if (image.mimeType.empty())
					{
						if (!image.uri.empty())
						{
							auto texture = ImageLoader::LoadTextureFromUri(image.uri);
						}
					}*/

					/*auto texture = ImageLoader::LoadTextureFromData(image.image.data(), image.image.size());
					if (texture);*/
				}
			}

			for (const auto& [attriName, aIdx] : primitive.attributes)
			{
				auto vaKind = VertexBuffer::NameToVBAttrKind(attriName);
				if (vaKind == VertexBufferAttriKind::INVALID)
				{
					continue;
				}
				const tinygltf::Accessor& accessor = tModel.accessors[aIdx];
				const tinygltf::BufferView& bufferView = tModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& tBuffer = tModel.buffers[bufferView.buffer];
				VertexBuffer* vBuffer = new VertexBuffer();
				{
					vBuffer->kind = vaKind;
					vBuffer->format = _GLTFTypeToIAFormat(accessor.type);
					vBuffer->byteStride = bufferView.byteStride;
					const unsigned char* tData = tBuffer.data.data() + bufferView.byteOffset;
					vBuffer->InitData(tData, bufferView.byteLength);
				}
				mc->GetGeometry()->AppendVertexBuffer(vBuffer);
			}

			// index buffer
			const tinygltf::Accessor& accessor = tModel.accessors[primitive.indices];
			const tinygltf::BufferView& bufferView = tModel.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& tBuffer = tModel.buffers[bufferView.buffer];
			const unsigned char* tData = tBuffer.data.data() + bufferView.byteOffset;
			if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			{
				std::uint16_t* pIndices = (std::uint16_t*)tData;
				std::vector<std::uint32_t> tmp(accessor.count);
				for (int i = 0; i < accessor.count; ++i)
				{
					tmp[i] = pIndices[i];
				}
				mc->Geometry_->GetIndexBuffer()->InitData(tmp.data(), tmp.size() * sizeof(tmp[0]));
			}
			else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
			{
				mc->Geometry_->GetIndexBuffer()->InitData(tData, accessor.count * sizeof(int));
			}
			else
			{
				Assert(false, "invalid index type");
			}

			model->AddComponment(mc);
		}
	}

	Entity* _CreateEntity(tinygltf::Model& tModel, const tinygltf::Node& tNode)
	{
		Entity* model = tNode.mesh == -1 ? new Entity() : new Model();
		
		if (tNode.translation.size() >= 3)
			model->GetPosition() = { (float)tNode.translation[0], (float)tNode.translation[1], (float)tNode.translation[2] };
		if (tNode.scale.size() >= 3)
			model->GetScale() = { (float)tNode.scale[0], (float)tNode.scale[1], (float)tNode.scale[2] };
		if (tNode.rotation.size() >= 4)
		{
			glm::quat rotation = { (float)tNode.rotation[0], (float)tNode.rotation[1], (float)tNode.rotation[2], (float)tNode.rotation[2] };
			model->GetRotation() = glm::degrees(glm::eulerAngles(rotation));
		}

		if (tNode.mesh != -1)
		{
			const tinygltf::Mesh& tMesh = tModel.meshes[tNode.mesh];
			_LoadPrimitives(model, tModel, tMesh);
		}

		// Load children
		for (const auto& idx : tNode.children)
		{
			const auto& node = tModel.nodes[idx];
			auto child = _CreateEntity(tModel, node);
			model->AddChild(child);
		}

		return model;
	}

	std::vector<Entity*> _LoadModelsFromGLTFScene(tinygltf::Model& tModel, const tinygltf::Scene& tScene)
	{
		std::vector<Entity*> models = {};
		for (std::uint32_t idx : tScene.nodes)
		{
			const tinygltf::Node& tNode = tModel.nodes[idx];
			
			auto model = _CreateEntity(tModel, tNode);

			/*Model* model = new Model();
			{
				if (tNode.translation.size() >= 3)
					model->GetPosition() = { (float)tNode.translation[0], (float)tNode.translation[1], (float)tNode.translation[2] };
				if (tNode.scale.size() >=3)
					model->GetScale() = { (float)tNode.scale[0], (float)tNode.scale[1], (float)tNode.scale[2] };
				if (tNode.rotation.size() >= 4)
				{
					glm::quat rotation = { (float)tNode.rotation[0], (float)tNode.rotation[1], (float)tNode.rotation[2], (float)tNode.rotation[2] };
					model->GetRotation() = glm::degrees(glm::eulerAngles(rotation));
				}
			}

			_LoadPrimitives(model, tModel, tMesh);*/

			models.push_back(model);

		}
		return models;
	}

	std::vector<Entity*> LoadModelFromGLTF(const std::string& filename)
	{
		tinygltf::Model tModel;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		std::string modelFile = FileSystem::GetInstance()->GetAbsFilePath(filename.c_str());

		bool ret = loader.LoadASCIIFromFile(&tModel, &err, &warn, modelFile);
		if (!ret) {
			return {};
		}

		for (const auto& tScene : tModel.scenes)
		{
			return _LoadModelsFromGLTFScene(tModel, tScene);
		}

		return {};
	}
}

NS_RX_END