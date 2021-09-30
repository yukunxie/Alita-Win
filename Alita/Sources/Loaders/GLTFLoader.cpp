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

	template<typename _TextureInfo>
	RHI::Texture* _LoadTexture(const tinygltf::Model& tModel, const _TextureInfo& info)
	{
		if (info.index == -1)
			return nullptr;
		const tinygltf::Image& image = tModel.images[tModel.textures[info.index].source];
		auto texture = _LoadTexture(image);
		/*if (texture)
		{
			mc->Material_->SetTexture("tAlbedo", texture);
		}*/
		/*if (image.mimeType.empty())
		{
			if (!image.uri.empty())
			{
				auto texture = ImageLoader::LoadTextureFromUri(image.uri);
			}
		}*/

		/*auto texture = ImageLoader::LoadTextureFromData(image.image.data(), image.image.size());
		if (texture);*/

		return texture;
	}

	void _LoadPrimitives(Entity* model, tinygltf::Model& tModel, const tinygltf::Mesh& tMesh)
	{
		for (const tinygltf::Primitive& primitive : tMesh.primitives)
		{
			bool useNormapMap = false;
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

				if (auto texture = _LoadTexture(tModel, pbrMetallicRoughness.baseColorTexture); texture)
				{
					mc->Material_->SetTexture("tAlbedo", texture);
				}
				if (auto texture = _LoadTexture(tModel, tMaterial.normalTexture); texture)
				{
					mc->Material_->SetTexture("tNormalMap", texture);
					useNormapMap = true;
				}
				if (auto texture = _LoadTexture(tModel, tMaterial.occlusionTexture); texture)
				{
					mc->Material_->SetTexture("tOcclusionMap", texture);
					useNormapMap = true;
				}
				if (auto texture = _LoadTexture(tModel, tMaterial.emissiveTexture); texture)
				{
					mc->Material_->SetTexture("tEmissiveMap", texture);
					useNormapMap = true;
				}
				if (auto texture = _LoadTexture(tModel, tMaterial.pbrMetallicRoughness.metallicRoughnessTexture); texture)
				{
					mc->Material_->SetTexture("tMetallicRoughnessMap", texture);
					useNormapMap = true;
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
			std::vector<std::uint32_t> tmpIndexData;
			const std::uint32_t* tmpPIndex = nullptr;
			const tinygltf::Accessor& accessor = tModel.accessors[primitive.indices];
			const tinygltf::BufferView& bufferView = tModel.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& tBuffer = tModel.buffers[bufferView.buffer];
			const unsigned char* tData = tBuffer.data.data() + bufferView.byteOffset;
			if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			{
				std::uint16_t* pIndices = (std::uint16_t*)tData;
				tmpIndexData.resize(accessor.count);
				for (int i = 0; i < accessor.count; ++i)
				{
					tmpIndexData[i] = pIndices[i];
				}
				tmpPIndex = tmpIndexData.data();
				mc->Geometry_->GetIndexBuffer()->InitData(tmpIndexData.data(), tmpIndexData.size() * sizeof(tmpIndexData[0]));
			}
			else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
			{
				tmpPIndex = (std::uint32_t*)tData;
				mc->Geometry_->GetIndexBuffer()->InitData(tData, accessor.count * sizeof(int));
			}
			else
			{
				Assert(false, "invalid index type");
			}

			// generate tangent and bitangent
			if (useNormapMap && !(mc->GetGeometry()->HasBiTangent() && mc->GetGeometry()->HasTangent()))
			{
				const uint8* pVertData = nullptr;
				const uint8* pUVData = nullptr;
				uint32 vertByteStride = 0;
				uint32 uvByteStride = 0;
				uint32 verticesCount = 0;

				// position
				{
					int aIdx = primitive.attributes.find("POSITION")->second;

					const tinygltf::Accessor& accessor = tModel.accessors[aIdx];
					const tinygltf::BufferView& bufferView = tModel.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& tBuffer = tModel.buffers[bufferView.buffer];
					pVertData = (tBuffer.data.data() + bufferView.byteOffset);

					vertByteStride = bufferView.byteStride;
					if (vertByteStride == 0)
					{
						vertByteStride = GetFormatSize(_GLTFTypeToIAFormat(accessor.type));
					}
					verticesCount = bufferView.byteLength / vertByteStride;
				}

				// uv
				{
					int aIdx = primitive.attributes.find("TEXCOORD_0")->second;

					const tinygltf::Accessor& accessor = tModel.accessors[aIdx];
					const tinygltf::BufferView& bufferView = tModel.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& tBuffer = tModel.buffers[bufferView.buffer];
					pUVData = (tBuffer.data.data() + bufferView.byteOffset);

					uvByteStride = bufferView.byteStride;
					if (uvByteStride == 0)
					{
						uvByteStride = GetFormatSize(_GLTFTypeToIAFormat(accessor.type));
					}
				}

				std::vector<TVector3> vertices(verticesCount);
				std::vector<TVector2> textureCoords(verticesCount);
				std::vector<TVector3> tangents(verticesCount);
				std::vector<TVector3> bitangents(verticesCount);

				for (uint32 i = 0; i < verticesCount; ++i)
				{
					const float* pVertices = (float*)pVertData;
					vertices[i] = TVector3{ pVertices[0], pVertices[1], pVertices[2] };
					pVertData += vertByteStride;

					const float* pUVs = (float*)pUVData;
					textureCoords[i] = TVector2{ pUVs [0], pUVs [1]};
					pUVData += uvByteStride;
				}

				const uint32 triangeCount = tModel.accessors[primitive.indices].count / 3;

				// https://learnopengl.com/Advanced-Lighting/Normal-Mapping
				for (uint32 triIdx = 0; triIdx < triangeCount; ++triIdx)
				{
					uint32 a = tmpPIndex[triIdx * 3 + 0];
					uint32 b = tmpPIndex[triIdx * 3 + 1];
					uint32 c = tmpPIndex[triIdx * 3 + 2];

					TVector3 edge1 = vertices[b] - vertices[a];
					TVector3 edge2 = vertices[c] - vertices[a];

					TVector2 deltaUV1 = textureCoords[b] - textureCoords[a];
					TVector2 deltaUV2 = textureCoords[c] - textureCoords[a];

					float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

					TVector3 tangent, bitangent;
					tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
					tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
					tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

					tangents[a] = tangent;
					tangents[b] = tangent;
					tangents[c] = tangent;

					bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
					bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
					bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

					bitangents[a] = bitangent;
					bitangents[b] = bitangent;
					bitangents[c] = bitangent;
				}

				if (!mc->GetGeometry()->HasTangent())
				{
					VertexBuffer* vBuffer = new VertexBuffer();
					{
						vBuffer->kind = VertexBufferAttriKind::TANGENT;
						vBuffer->format = InputAttributeFormat::FLOAT3;
						vBuffer->byteStride = 0;
						vBuffer->InitData(tangents.data(), sizeof(tangents[0]) * tangents.size());
					}
					mc->GetGeometry()->AppendVertexBuffer(vBuffer);
				}

				if (!mc->GetGeometry()->HasBiTangent())
				{
					VertexBuffer* vBuffer = new VertexBuffer();
					{
						vBuffer->kind = VertexBufferAttriKind::BITANGENT;
						vBuffer->format = InputAttributeFormat::FLOAT3;
						vBuffer->byteStride = 0;
						vBuffer->InitData(bitangents.data(), sizeof(bitangents[0]) * bitangents.size());
					}
					mc->GetGeometry()->AppendVertexBuffer(vBuffer);
				}
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
			// !! caution: wxyz!!
			glm::quat rotation = { (float)tNode.rotation[3], (float)tNode.rotation[0], (float)tNode.rotation[1], (float)tNode.rotation[2] };
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