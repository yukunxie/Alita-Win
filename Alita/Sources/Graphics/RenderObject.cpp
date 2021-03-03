#include "RenderObject.h"

NS_RX_BEGIN

void RenderObject::Render(RHI::RenderPassEncoder& passEndcoder)
{
	materialObject->Apply(passEndcoder);

	for (const auto& vb : vertexBuffers)
	{
		passEndcoder.SetVertexBuffer(vb.gpuBuffer, vb.offset, vb.slot);
	}

	passEndcoder.SetIndexBuffer(indexBuffer.gpuBuffer, indexBuffer.offset);

	if (indexBuffer.instanceCount == 0)
	{
		passEndcoder.DrawIndexed(indexBuffer.indexCount, 1, 0, 0, 0);
	}
}

NS_RX_END