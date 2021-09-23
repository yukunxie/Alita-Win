#pragma once

#include "RHI/RHI.h"

#include "Base/ObjectBase.h"
#include "Meshes/VertexBuffer.h"
#include "Material.h"
#include "Pass.h"


NS_RX_BEGIN

struct RenderObject
{
	void Render(const Pass* pass, ETechniqueType technique, ERenderSet renderSet, RHI::RenderPassEncoder& passEndcoder);

	struct VertexBufferInfo
	{
		RHI::Buffer* gpuBuffer = nullptr;
		std::uint32_t	offset = 0;
		std::uint32_t	slot = 0xFFFFFFFF;
	};

	struct IndexBufferInfo
	{
		RHI::Buffer* gpuBuffer = nullptr;
		std::uint32_t	offset = 0;
		std::uint32_t	indexCount = 0;
		std::uint32_t	instanceCount = 0;
		IndexType		indexType = IndexType::UINT32;

	};

	Material* materialObject;
	std::vector<VertexBufferInfo> vertexBuffers;
	IndexBufferInfo indexBuffer;
};

NS_RX_END
