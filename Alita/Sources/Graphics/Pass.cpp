//
// Created by realxie on 2019-10-29.
//

#include "Pass.h"

NS_RX_BEGIN

void IgniterPass::Execute(RHI::CommandEncoder* cmdEncoder)
{
	RHI::RenderPassDescriptor renderPassDescriptor;

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
}

NS_RX_END
