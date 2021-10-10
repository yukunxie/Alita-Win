//
// Created by realxie on 2019-10-29.
//

#include "MeshComponent.h"
#include "Base/Entity.h"
#include "Graphics/RenderScene.h"

NS_RX_BEGIN

void MeshComponent::SetupRenderObject()
{
	RenderObject_.bSelected = IsSelected();

	if (RenderObject_.MaterialObject)
	{
		return;
	}
	RenderObject_.RenderSetBits = RenderSetBits_;
	RenderObject_.MaterialObject = Material_;
	auto& ib = RenderObject_.IndexBuffer;
	{
		ib.GpuBuffer = Geometry_->GetIndexBuffer()->gpuBuffer;
		ib.IndexCount = Geometry_->GetIndexBuffer()->GetIndexCount();
		ib.InstanceCount = 0;
		ib.IndexType = IndexType::UINT32;
		ib.Offset = 0;
	}

	//InputAssembler 

	std::vector<InputAttribute> attributes;

	RenderObject_.VertexBuffers.clear();

	uint32 slot = 0;

	for (auto& vbs : Geometry_->GetVBStreams())
	{
		RenderObject::VertexBufferInfo vb;
		{
			vb.GpuBuffer = vbs->gpuBuffer;
			vb.Offset = 0;
			vb.Slot = slot++;
		}
		RenderObject_.VertexBuffers.push_back(vb);

		InputAttribute iAttri;
		{
			iAttri.format = vbs->format;
			iAttri.kind = vbs->kind;
			iAttri.location = vb.Slot;
			iAttri.stride = vbs->byteStride ? vbs->byteStride : GetFormatSize(iAttri.format);
		}
		attributes.push_back(iAttri);
	}

	Material_->SetInputAssembler({ attributes , IndexType::UINT32 });
}

void MeshComponent::Tick(float dt)
{
	SetupRenderObject();

	const auto* etOwner = GetOwner();

	const TMat4x4 worldMatrix = etOwner->GetWorldMatrix();
	Material_->SetFloat("WorldMatrix", 0, 16, (float*)&worldMatrix);

	Engine::GetRenderScene()->AddRenderObject(&RenderObject_);
}

MeshComponent* MeshComponentBuilder::CreateBox(const std::string& material)
{
	MeshComponent* meshComp = new MeshComponent();
	meshComp->Geometry_ = new Geometry;
	meshComp->Material_ = new Material(material.empty() ? "Materials/PBR.json" : material);

	std::vector<TVector3> positions = {
		// Front face
		{-1.0f, -1.0f, 1.0f},
		{1.0f,  -1.0f, 1.0f},
		{1.0f,  1.0f,  1.0f},
		{-1.0f, 1.0f,  1.0f},
		// Back face
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f,  -1.0f},
		{1.0f,  1.0f,  -1.0f},
		{1.0f,  -1.0f, -1.0f},
		// Top face
		{-1.0f, 1.0f,  -1.0f},
		{-1.0f, 1.0f,  1.0f},
		{1.0f,  1.0f,  1.0f},
		{1.0f,  1.0f,  -1.0f},
		// Bottom face
		{-1.0f, -1.0f, -1.0f},
		{1.0f,  -1.0f, -1.0f},
		{1.0f,  -1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		// Right face
		{1.0f,  -1.0f, -1.0f},
		{1.0f,  1.0f,  -1.0f},
		{1.0f,  1.0f,  1.0f},
		{1.0f,  -1.0f, 1.0f},
		// Left face
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f,  1.0f},
		{-1.0f, 1.0f,  -1.0f},
	};

	std::vector<TVector3> colors = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
	};

	std::vector<TVector2> texCoords =
	{
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f}
	};

	auto BindVertexBufferHandle = [meshComp](VertexBufferAttriKind kind, InputAttributeFormat format, const void* data, uint32 size)
	{
		auto vbBuffer = new VertexBuffer();
		vbBuffer->kind = kind;
		vbBuffer->format = format;
		vbBuffer->InitData(data, size);
		meshComp->Geometry_->AppendVertexBuffer(vbBuffer);
	};

	BindVertexBufferHandle(VertexBufferAttriKind::POSITION, InputAttributeFormat::FLOAT3, positions.data(), sizeof(positions[0]) * positions.size());
	BindVertexBufferHandle(VertexBufferAttriKind::DIFFUSE, InputAttributeFormat::FLOAT3, colors.data(), sizeof(colors[0])* colors.size());
	BindVertexBufferHandle(VertexBufferAttriKind::TEXCOORD, InputAttributeFormat::FLOAT2, texCoords.data(), sizeof(texCoords[0])* texCoords.size());

	std::vector<std::uint32_t> indices = {
		// front
		0, 1, 2, 0, 2, 3,
		// back
		4, 5, 6, 4, 6, 7,
		// top
		8, 9, 10, 8, 10, 11,
		// bottom
		12, 13, 14, 12, 14, 15,
		// right
		16, 17, 18, 16, 18, 19,
		// left
		20, 21, 22, 20, 22, 23,
	};

	std::vector<TVector3> normals(positions.size());

	for (uint32 face = 0; face < 6; ++face)
	{
		uint32 idx = face * 6;
		const TVector3& v0 = positions[indices[idx]];
		const TVector3& v1 = positions[indices[idx + 1]];
		const TVector3& v2 = positions[indices[idx + 2]];

		TVector3 d0 = v1 - v0;
		TVector3 d1 = v2 - v0;

		TVector3 normal = -glm::normalize(glm::cross(d1, d0));

		for (uint32 i = 0; i < 6; i++)
		{
			normals[indices[idx + i]] = normal;
		}
	}
	BindVertexBufferHandle(VertexBufferAttriKind::NORMAL, InputAttributeFormat::FLOAT3, normals.data(), sizeof(normals[0])* normals.size());

	meshComp->Geometry_->indexBuffer_.indexType = IndexType::UINT32;
	meshComp->Geometry_->indexBuffer_.InitData(indices.data(), indices.size() * sizeof(indices[0]));
	

	return meshComp;
}

NS_RX_END