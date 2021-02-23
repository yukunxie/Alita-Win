//
// Created by realxie on 2019-10-29.
//

#include "MeshComponent.h"

NS_RX_BEGIN


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
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,


		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,


		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
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

	//// test
	//const std::vector<float> vertices = {
	//	// Front face
	//	-1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
	//	1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
	//	1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	//	-1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

	//	// Back face
	//	-1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
	//	-1.0f, 1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
	//	1.0f,  1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	//	1.0f,  -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

	//	// Top face
	//	-1.0f, 1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
	//	-1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
	//	1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	//	1.0f,  1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

	//	// Bottom face
	//	-1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
	//	1.0f,  -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
	//	1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	//	-1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

	//	// Right face
	//	1.0f,  -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
	//	1.0f,  1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
	//	1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	//	1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

	//	// Left face
	//	-1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
	//	-1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
	//	-1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	//	-1.0f, 1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f
	//};

	auto BindVertexBufferHandle = [meshComp](VertexBufferAttriKind kind, const std::vector<float>& data)
	{
		auto vbBuffer = new VertexBuffer();
		vbBuffer->kind = kind;
		vbBuffer->InitData(data.data(), data.size() * sizeof(data[0]));
		meshComp->geometry_->AppendVertexBuffer(vbBuffer);
	};

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