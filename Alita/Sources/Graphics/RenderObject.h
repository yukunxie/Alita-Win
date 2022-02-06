#pragma once

#include "GFX/GFX.h"

#include "Base/ObjectBase.h"
#include "Meshes/VertexBuffer.h"
#include "Material.h"
#include "Pass.h"


NS_RX_BEGIN

class RenderObject
{
public:
	void Render(const Pass* pass, ETechniqueType technique, ERenderSet renderSet, gfx::RenderPassEncoder& passEndcoder);

	struct VertexBufferInfo
	{
		gfx::BufferPtr	GpuBuffer = nullptr;
		std::uint32_t	Offset = 0;
		std::uint32_t	Slot = 0xFFFFFFFF;
	};

	struct IndexBufferInfo
	{
		gfx::BufferPtr	GpuBuffer = nullptr;
		std::uint32_t	Offset = 0;
		std::uint32_t	IndexCount = 0;
		std::uint32_t	InstanceCount = 0;
		IndexType		IndexType = IndexType::UINT32;
	};

	Material* MaterialObject;
	std::vector<VertexBufferInfo> VertexBuffers;
	IndexBufferInfo IndexBuffer;
	std::uint64_t	RenderSetBits = 0;

	uint8			bSelected : 1;
};

NS_RX_END
