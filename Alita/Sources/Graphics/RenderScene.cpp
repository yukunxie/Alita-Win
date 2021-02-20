#include "RenderScene.h"
#include "Engine/Engine.h"

NS_RX_BEGIN

RenderScene::RenderScene()
{
	rhiCommandEncoder_ = Engine::GetEngine()->GetGPUDevice()->CreateCommandEncoder();
}

RenderScene::~RenderScene()
{

}

void RenderScene::AddPrimitive(MeshComponent* mesh)
{
	if (!rhiVertexBuffer_)
	{
		// Create VertexBuffer
		{
			// 1M bytes
			RHI::BufferSize vertexBufferSize = 1024 * 1024;

			RHI::BufferDescriptor vertexBufferDescriptor;
			{
				vertexBufferDescriptor.usage = RHI::BufferUsage::VERTEX;
				vertexBufferDescriptor.size = vertexBufferSize;
			}

			rhiVertexBuffer_ = Engine::GetEngine()->GetGPUDevice()->CreateBuffer(vertexBufferDescriptor);
		}

		{
			RHI::BufferSize indexBufferSize = 10 * 1024;
			RHI::BufferDescriptor indexBufferDescriptor;
			{
				indexBufferDescriptor.usage = RHI::BufferUsage::INDEX;
				indexBufferDescriptor.size = indexBufferSize;
			}
			rhiIndexBuffer_ = Engine::GetEngine()->GetGPUDevice()->CreateBuffer(indexBufferDescriptor);

		}

	}

	std::uint8_t* pVertexData = (std::uint8_t*)rhiVertexBuffer_->MapWriteAsync();
	auto geometry = mesh->GetGeometry();
	VertexBuffer* buffer = geometry->GetVBStreams()[0];
	memcpy(pVertexData, buffer->buffer.data(), buffer->buffer.size());
	rhiVertexBuffer_->Unmap();


	std::uint8_t* pIndexData = (std::uint8_t*)rhiIndexBuffer_->MapWriteAsync();
	memcpy(pIndexData, geometry->GetIndexBuffer()->data(), geometry->GetIndexBuffer()->size());
	rhiIndexBuffer_->Unmap();
}

void RenderScene::SubmitGPU()
{
	std::vector<RHI::RenderPassColorAttachmentDescriptor> colorAttachments = {
	 RHI::RenderPassColorAttachmentDescriptor{
		 .attachment = Engine::GetEngine()->GetSwapchain()->GetCurrentTexture(),
		 .resolveTarget = nullptr,
		 .loadValue = {1.0f, 1.0f, 0.0f, 1.0f},
		 .loadOp = RHI::LoadOp::CLEAR,
		 .storeOp = RHI::StoreOp::STORE,
	 }
	};

	RHI::RenderPassDescriptor renderPassDescriptor;
	renderPassDescriptor.colorAttachments = std::move(colorAttachments);
	renderPassDescriptor.depthStencilAttachment = {
		.attachment = nullptr,
		.depthLoadOp = RHI::LoadOp::CLEAR,
		.depthStoreOp = RHI::StoreOp::STORE,
		.depthLoadValue = 1.0f,
		.stencilLoadOp = RHI::LoadOp::CLEAR,
		.stencilStoreOp = RHI::StoreOp::STORE,
		.stencilLoadValue = 0,
	};

	auto renderPassEncoder = rhiCommandEncoder_->BeginRenderPass(renderPassDescriptor);

	renderPassEncoder->EndPass();

	auto commandBuffer = rhiCommandEncoder_->Finish();

	Engine::GetEngine()->GetGPUDevice()->GetQueue()->Submit(commandBuffer);

	// Render to screen.
	Engine::GetEngine()->GetSwapchain()->Present(Engine::GetEngine()->GetGPUDevice()->GetQueue());
}

NS_RX_END