#include "RenderObject.h"

NS_RX_BEGIN

void RenderObject::Render(const Pass* pass, ETechniqueType technique, ERenderSet renderSet, RHI::RenderPassEncoder& passEndcoder)
{
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