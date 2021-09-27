#include "RenderObject.h"

NS_RX_BEGIN

bool CheckSkipRenderObjects(ETechniqueType technique, std::uint64_t renderSetBits)
{
	switch (technique)
	{
	case ETechniqueType::TShading:
		return false;
	case ETechniqueType::TGBufferGen:
	case ETechniqueType::TShadowMapGen:
		return !(ERenderSet_Opaque & renderSetBits);
	case ETechniqueType::TSkyBox:
		return !(ERenderSet_SkyBox & renderSetBits);
	}
	RHI_ASSERT(false);
	return false;
}

void RenderObject::Render(const Pass* pass, ETechniqueType technique, ERenderSet renderSet, RHI::RenderPassEncoder& passEndcoder)
{
	if (CheckSkipRenderObjects(technique, this->RenderSetBits))
	{
		return;
	}
	MaterialObject->Apply(pass, technique, renderSet, passEndcoder);

	uint32 slot = 0;
	for (const auto& vb : VertexBuffers)
	{
		passEndcoder.SetVertexBuffer(vb.GpuBuffer, vb.Offset, slot++/*vb.slot*/);
	}

	passEndcoder.SetIndexBuffer(IndexBuffer.GpuBuffer, IndexBuffer.Offset);

	if (IndexBuffer.InstanceCount == 0)
	{
		passEndcoder.DrawIndexed(IndexBuffer.IndexCount, 1, 0, 0, 0);
	}
}

NS_RX_END