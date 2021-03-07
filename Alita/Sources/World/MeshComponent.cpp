//
// Created by realxie on 2019-10-29.
//

#include "MeshComponent.h"
#include "Base/Entity.h"
#include "Graphics/RenderScene.h"

NS_RX_BEGIN

void MeshComponent::SetupRenderObject()
{
	if (renderObject_.materialObject)
	{
		return;
	}
	renderObject_.materialObject = material_;
	auto& ib = renderObject_.indexBuffer;
	{
		ib.gpuBuffer = geometry_->GetIndexBuffer()->gpuBuffer;
		ib.indexCount = geometry_->GetIndexBuffer()->GetIndexCount();
		ib.instanceCount = 0;
		ib.indexType = IndexType::UINT32;
		ib.offset = 0;
	}

	//InputAssembler 

	std::vector<InputAttribute> attributes;

	renderObject_.vertexBuffers.clear();

	for (auto& vbs : geometry_->GetVBStreams())
	{
		RenderObject::VertexBufferInfo vb;
		{
			vb.gpuBuffer = vbs->gpuBuffer;
			vb.offset = 0;
			vb.slot = GetInputAttributeLocation(vbs->kind);
		}
		renderObject_.vertexBuffers.push_back(vb);

		InputAttribute iAttri;
		{
			iAttri.format = vbs->format;
			iAttri.kind = vbs->kind;
			iAttri.location = vb.slot;
			iAttri.stride = vbs->byteStride ? vbs->byteStride : GetFormatSize(iAttri.format);
		}
		attributes.push_back(iAttri);
	}

	material_->SetInputAssembler({ attributes , IndexType::UINT32 });


	/*const auto& IAs = material_->GetInputAttributes();
	for (const auto& IA : IAs)
	{
		for (auto& vbs : geometry_->GetVBStreams())
		{
			if (vbs->kind == IA.kind)
			{
				RenderObject::VertexBufferInfo vb;
				{
					vb.gpuBuffer = vbs->gpuBuffer;
					vb.offset = 0;
					vb.slot = IA.location;
				}
				renderObject_.vertexBuffers.push_back(vb);
			}
		}
	}*/
	//Assert(renderObject_.vertexBuffers.size() == IAs.size(), "invalid vertex buffer count.");
}

void MeshComponent::Tick(float dt)
{
	SetupRenderObject();

	const auto* etOwner = GetOwner();

	const TMat4x4 worldMatrix = etOwner->GetWorldMatrix();
	material_->SetFloat("WorldMatrix", 0, 16, (float*)&worldMatrix);

	Engine::GetRenderScene()->AddRenderObject(&renderObject_);
}

MeshComponent* MeshComponentBuilder::CreateBox()
{
	MeshComponent* meshComp = new MeshComponent();
	meshComp->geometry_ = new Geometry;
	meshComp->material_ = new Material("Shaders/CommonMaterial.json");

	std::vector<float> positions = {
		// Front face
		-1.0f, -1.0f, 1.0f,
		1.0f,  -1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f, 1.0f,  1.0f,
		// Back face
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f,  -1.0f,
		1.0f,  1.0f,  -1.0f,
		1.0f,  -1.0f, -1.0f,
		// Top face
		-1.0f, 1.0f,  -1.0f,
		-1.0f, 1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  -1.0f,
		// Bottom face
		-1.0f, -1.0f, -1.0f,
		1.0f,  -1.0f, -1.0f,
		1.0f,  -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		// Right face
		1.0f,  -1.0f, -1.0f,
		1.0f,  1.0f,  -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  -1.0f, 1.0f,
		// Left face
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f,  1.0f,
		-1.0f, 1.0f,  -1.0f,
	};

	std::vector<float> colors = {
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,


		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,


		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
	};

	std::vector<float> texCoords =
	{
		.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	auto BindVertexBufferHandle = [meshComp](VertexBufferAttriKind kind, const std::vector<float>& data)
	{
		auto vbBuffer = new VertexBuffer();
		vbBuffer->kind = kind;
		vbBuffer->InitData(data.data(), data.size() * sizeof(data[0]));
		meshComp->geometry_->AppendVertexBuffer(vbBuffer);
	};

	BindVertexBufferHandle(VertexBufferAttriKind::POSITION, positions);
	BindVertexBufferHandle(VertexBufferAttriKind::NORMAL, colors);
	BindVertexBufferHandle(VertexBufferAttriKind::TEXCOORD, texCoords);

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

	meshComp->geometry_->indexBuffer_.indexType = IndexType::UINT32;
	meshComp->geometry_->indexBuffer_.InitData(indices.data(), indices.size() * sizeof(indices[0]));
	

	return meshComp;
}

NS_RX_END