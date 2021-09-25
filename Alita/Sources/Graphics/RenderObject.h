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
		RHI::Buffer*	GpuBuffer = nullptr;
		std::uint32_t	Offset = 0;
		std::uint32_t	Slot = 0xFFFFFFFF;
	};

	struct IndexBufferInfo
	{
		RHI::Buffer*	GpuBuffer = nullptr;
		std::uint32_t	Offset = 0;
		std::uint32_t	IndexCount = 0;
		std::uint32_t	InstanceCount = 0;
		IndexType		IndexType = IndexType::UINT32;
	};

	Material* MaterialObject;
	std::vector<VertexBufferInfo> VertexBuffers;
	IndexBufferInfo IndexBuffer;
	std::uint64_t	RenderFlags = 0;
};

NS_RX_END
