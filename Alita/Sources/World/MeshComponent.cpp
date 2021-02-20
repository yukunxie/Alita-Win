//
// Created by realxie on 2019-10-29.
//

#include "MeshComponent.h"

NS_RX_BEGIN


MeshComponent* MeshComponentBuilder::CreateBox()
{
	MeshComponent* meshComp = new MeshComponent();
	meshComp->geometry_ = new Geometry;

	// test
	const std::vector<float> vertices = {
		// Front face
		-1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
		1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
		-1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

		// Back face
		-1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
		-1.0f, 1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
		1.0f,  1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
		1.0f,  -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

		// Top face
		-1.0f, 1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
		-1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
		1.0f,  1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

		// Bottom face
		-1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
		1.0f,  -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
		1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

		// Right face
		1.0f,  -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
		1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
		1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

		// Left face
		-1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
		-1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
		-1.0f, 1.0f,  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f
	};

	auto vbBuffer = new VertexBuffer();
	vbBuffer->mask = VertexBufferAttriKind::POSITION | VertexBufferAttriKind::DIFFUSE | VertexBufferAttriKind::TEXCOORD;
	vbBuffer->buffer.resize(vertices.size() * sizeof(float));
	memcpy(vbBuffer->buffer.data(), vertices.data(), vbBuffer->buffer.size());

	meshComp->geometry_->AppendVertexBuffer(vbBuffer);
	meshComp->geometry_->indexBuffer_ = {
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

	return meshComp;
}

NS_RX_END