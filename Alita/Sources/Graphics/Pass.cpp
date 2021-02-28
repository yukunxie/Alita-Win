//
// Created by realxie on 2019-10-29.
//

#include "Pass.h"
#include "World/MeshComponent.h"
#include "World/World.h"
#include "World/Camera.h"

NS_RX_BEGIN

void IgniterPass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<MeshComponent*>& meshComponents)
{
	/*RHI::RenderPassDescriptor renderPassDescriptor;

	for (const auto& tp : attachments_)
	{
		RHI::RenderPassColorAttachmentDescriptor descriptor;
		{
			descriptor.attachment = tp.second;
			descriptor.resolveTarget = nullptr;
			descriptor.loadValue = { 0.0f, 1.0f, 0.0f, 1.0f };
			descriptor.loadOp = RHI::LoadOp::CLEAR;
			descriptor.storeOp = RHI::StoreOp::STORE;
		}
		renderPassDescriptor.colorAttachments.push_back(descriptor);
	}

	renderPassDescriptor.depthStencilAttachment = {
		.attachment = dsAttachment_,
		.depthLoadOp = RHI::LoadOp::CLEAR,
		.depthStoreOp = RHI::StoreOp::STORE,
		.depthLoadValue = 1.0f,
		.stencilLoadOp = RHI::LoadOp::CLEAR,
		.stencilStoreOp = RHI::StoreOp::STORE,
		.stencilLoadValue = 0,
	};

	auto renderPassEncoder = cmdEncoder->BeginRenderPass(renderPassDescriptor);

	renderPassEncoder->EndPass();

	Reset();*/
}

void OpaquePass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<MeshComponent*>& meshComponents)
{
	RHI::RenderPassDescriptor renderPassDescriptor;

	for (const auto& tp : attachments_)
	{
		RHI::RenderPassColorAttachmentDescriptor descriptor;
		{
			descriptor.attachment = tp.second;
			descriptor.resolveTarget = nullptr;
			descriptor.loadValue = { 1.0f, 0.0f, 0.0f, 1.0f };
			descriptor.loadOp = RHI::LoadOp::CLEAR;
			descriptor.storeOp = RHI::StoreOp::STORE;
		}
		renderPassDescriptor.colorAttachments.push_back(descriptor);
	}

	renderPassDescriptor.depthStencilAttachment = {
		.attachment = dsAttachment_,
		.depthLoadOp = RHI::LoadOp::CLEAR,
		.depthStoreOp = RHI::StoreOp::STORE,
		.depthLoadValue = 1.0f,
		.stencilLoadOp = RHI::LoadOp::CLEAR,
		.stencilStoreOp = RHI::StoreOp::STORE,
		.stencilLoadValue = 0,
	};

	auto renderPassEncoder = cmdEncoder->BeginRenderPass(renderPassDescriptor);

	for (const auto mesh : meshComponents)
	{
		auto material = mesh->GetMaterial();
		auto geometry = mesh->GetGeometry();

		{
			TMat4x4 modelMatrix(1.0f);
			const World* world = Engine::GetEngine()->GetWorld();
			const TMat4x4& viewMatrix = world->GetCamera()->GetViewMatrix();
			const TMat4x4& projMatrix = world->GetCamera()->GetProjectionMatrix();

			material->SetFloat("model", 0, 16, (float*)&modelMatrix);
			material->SetFloat("view", 0, 16, (float*)&viewMatrix);
			material->SetFloat("proj", 0, 16, (float*)&projMatrix);

			material->Apply(*renderPassEncoder);

			int idx = 0;
			for (auto vb : geometry->GetVBStreams())
			{
				renderPassEncoder->SetVertexBuffer(vb->gpuBuffer, 0, idx++);
			}
			renderPassEncoder->SetIndexBuffer(geometry->GetIndexBuffer()->gpuBuffer, 0);

			const RHI::Extent2D extent = { 1280, 800 };
			renderPassEncoder->SetViewport(0, 0, extent.width, extent.height, 0, 1);
			renderPassEncoder->SetScissorRect(0, 0, extent.width, extent.height);
			renderPassEncoder->DrawIndxed(36, 0);
		}
	}

	renderPassEncoder->EndPass();

	Reset();

}

NS_RX_END
