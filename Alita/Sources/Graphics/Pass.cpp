//
// Created by realxie on 2019-10-29.
//

#include "Pass.h"
#include "World/MeshComponent.h"
#include "World/World.h"
#include "World/Camera.h"
#include "RenderObject.h"

NS_RX_BEGIN

void IgniterPass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects)
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

ShadowMapGenPass::ShadowMapGenPass()
{
	{
		RHI::TextureDescriptor descriptor;
		{
			descriptor.sampleCount = 1;
			descriptor.format = RHI::TextureFormat::DEPTH24PLUS_STENCIL8;
			descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
			descriptor.size = { shadowMapSize_.width, shadowMapSize_.height };
			descriptor.arrayLayerCount = 1;
			descriptor.mipLevelCount = 1;
			descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		};
		dsTexture_ = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView();
	}

	{
		RHI::TextureDescriptor descriptor;
		{
			descriptor.sampleCount = 1;
			descriptor.format = RHI::TextureFormat::R32FLOAT;
			descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
			descriptor.size = { shadowMapSize_.width, shadowMapSize_.height };
			descriptor.arrayLayerCount = 1;
			descriptor.mipLevelCount = 1;
			descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		};
		shadowMapTexture_ = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView();
	}
}

void ShadowMapGenPass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects)
{
	RHI::RenderPassDescriptor renderPassDescriptor;

	RHI::RenderPassColorAttachmentDescriptor descriptor;
	{
		descriptor.attachment = shadowMapTexture_;
		descriptor.resolveTarget = nullptr;
		descriptor.loadValue = { 1.0f, 1.0f, 1.0f, 1.0f };
		descriptor.loadOp = RHI::LoadOp::CLEAR;
		descriptor.storeOp = RHI::StoreOp::STORE;
	}
	renderPassDescriptor.colorAttachments.push_back(descriptor);

	renderPassDescriptor.depthStencilAttachment = {
		.attachment = dsTexture_,
		.depthLoadOp = RHI::LoadOp::CLEAR,
		.depthStoreOp = RHI::StoreOp::STORE,
		.depthLoadValue = 1.0f,
		.stencilLoadOp = RHI::LoadOp::CLEAR,
		.stencilStoreOp = RHI::StoreOp::STORE,
		.stencilLoadValue = 0,
	};

	auto renderPassEncoder = cmdEncoder->BeginRenderPass(renderPassDescriptor);

	const RHI::Extent2D extent = { 1280, 800 };
	renderPassEncoder->SetViewport(0, 0, extent.width, extent.height, 0, 1);
	renderPassEncoder->SetScissorRect(0, 0, extent.width, extent.height);

	for (const auto ro : renderObjects)
	{
		ro->Render(*renderPassEncoder);
	}

	renderPassEncoder->EndPass();
}

void OpaquePass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects)
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

	const RHI::Extent2D extent = { 1280, 800 };
	renderPassEncoder->SetViewport(0, 0, extent.width, extent.height, 0, 1);
	renderPassEncoder->SetScissorRect(0, 0, extent.width, extent.height);

	for (const auto ro : renderObjects)
	{
		ro->Render(*renderPassEncoder);
	}

	renderPassEncoder->EndPass();

	Reset();

}

NS_RX_END
