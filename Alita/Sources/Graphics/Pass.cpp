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
	//{
	//	RHI::TextureDescriptor descriptor;
	//	{
	//		descriptor.sampleCount = 1;
	//		descriptor.format = RHI::TextureFormat::DEPTH24PLUS_STENCIL8;
	//		descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
	//		descriptor.size = { shadowMapSize_.width, shadowMapSize_.height };
	//		descriptor.arrayLayerCount = 1;
	//		descriptor.mipLevelCount = 1;
	//		descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
	//	};
	//	dsTexture_ = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView();
	//}

	//{
	//	RHI::TextureDescriptor descriptor;
	//	{
	//		descriptor.sampleCount = 1;
	//		descriptor.format = RHI::TextureFormat::R32FLOAT;
	//		descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
	//		descriptor.size = { shadowMapSize_.width, shadowMapSize_.height };
	//		descriptor.arrayLayerCount = 1;
	//		descriptor.mipLevelCount = 1;
	//		descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
	//	};
	//	shadowMapTexture_ = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView();
	//}
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
		ro->Render(this, ETechniqueType::TShading, ERenderSet::ERenderSet_Opaque, *renderPassEncoder);
	}

	renderPassEncoder->EndPass();
}

DeferredPass::DeferredPass()
{
	const RHI::Extent2D extent = { 1280, 800 };

	// 0
	{
		RHI::TextureDescriptor descriptor;
		descriptor.sampleCount = 1;
		descriptor.format = RHI::TextureFormat::RGBA16FLOAT;
		descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
		descriptor.size = extent;
		descriptor.arrayLayerCount = 1;
		descriptor.mipLevelCount = 1;
		descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		GBuffers_.GDiffuse = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView({});
	};

	// 1
	{
		RHI::TextureDescriptor descriptor;
		descriptor.sampleCount = 1;
		descriptor.format = RHI::TextureFormat::RGBA16FLOAT;
		descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
		descriptor.size = extent;
		descriptor.arrayLayerCount = 1;
		descriptor.mipLevelCount = 1;
		descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		GBuffers_.GNormal = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView({});
	};

	// 2
	{
		RHI::TextureDescriptor descriptor;
		descriptor.sampleCount = 1;
		descriptor.format = RHI::TextureFormat::RGBA16FLOAT;
		descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
		descriptor.size = extent;
		descriptor.arrayLayerCount = 1;
		descriptor.mipLevelCount = 1;
		descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		GBuffers_.GPosition = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView({});
	};

	// 3
	{
		RHI::TextureDescriptor descriptor;
		descriptor.sampleCount = 1;
		descriptor.format = RHI::TextureFormat::RGBA16FLOAT;
		descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
		descriptor.size = extent;
		descriptor.arrayLayerCount = 1;
		descriptor.mipLevelCount = 1;
		descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		GBuffers_.GMaterial = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView({});
	};

	// ds
	{
		RHI::TextureDescriptor descriptor;
		descriptor.sampleCount = 1;
		descriptor.format = RHI::TextureFormat::DEPTH24PLUS_STENCIL8;
		descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
		descriptor.size = extent;
		descriptor.arrayLayerCount = 1;
		descriptor.mipLevelCount = 1;
		descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		dsTexture_ = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView({});
	};
}

void DeferredPass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects)
{

}

OpaquePass::OpaquePass()
{
	const RHI::Extent2D extent = { 1280, 800 };
	// 0
	{
		RHI::TextureDescriptor descriptor;
		descriptor.sampleCount = 1;
		descriptor.format = RHI::TextureFormat::RGBA16FLOAT;
		descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
		descriptor.size = extent;
		descriptor.arrayLayerCount = 1;
		descriptor.mipLevelCount = 1;
		descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		rtColor_ = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView({});
		SetupOutputAttachment(0, rtColor_);
	};

	// ds
	{
		RHI::TextureDescriptor descriptor;
		descriptor.sampleCount = 1;
		descriptor.format = RHI::TextureFormat::DEPTH24PLUS_STENCIL8;
		descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
		descriptor.size = extent;
		descriptor.arrayLayerCount = 1;
		descriptor.mipLevelCount = 1;
		descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		rtDepthStencil_ = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView({});
	};
}

void OpaquePass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects)
{
	RHI::RenderPassDescriptor renderPassDescriptor;

	SetupOutputAttachment(0, rtColor_);
	SetupDepthStencilAttachemnt(rtDepthStencil_);

	for (const auto& tp : attachments_)
	{
		RHI::RenderPassColorAttachmentDescriptor descriptor;
		{
			descriptor.attachment = rtColor_;
			descriptor.resolveTarget = nullptr;
			descriptor.loadValue = { 1.0f, 1.0f, 0.0f, 1.0f };
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
		ro->Render(this, ETechniqueType::TShading, ERenderSet::ERenderSet_Opaque, *renderPassEncoder);
	}

	renderPassEncoder->EndPass();
}

PostProcessBasePass::PostProcessBasePass(const std::string& shaderName, ETechniqueType technique)
{
	MeshComponent* meshComp = new MeshComponent();
	meshComp->geometry_ = new Geometry;
	meshComp->material_ = new Material("Materials/PostProcess.json");

	{
		std::vector<float> vertices = {-1, -1, 0,	// bottom left corner
							-1,  1, 0,	// top left corner
							 1,  1, 0,	// top right corner
							 1, -1, 0 };// bottom right corner

		auto vbBuffer = new VertexBuffer();
		vbBuffer->kind = VertexBufferAttriKind::POSITION;
		vbBuffer->format = InputAttributeFormat::FLOAT3;
		vbBuffer->InitData(vertices.data(), vertices.size() * sizeof(vertices[0]));
		meshComp->geometry_->AppendVertexBuffer(vbBuffer);
	}

	{
		std::vector<float> texCoords = { 0, 0,	// bottom left corner
							0,  1,	// top left corner
							 1,  1,	// top right corner
							 1, 0 };// bottom right corner

		auto vbBuffer = new VertexBuffer();
		vbBuffer->kind = VertexBufferAttriKind::TEXCOORD;
		vbBuffer->format = InputAttributeFormat::FLOAT2;
		vbBuffer->InitData(texCoords.data(), texCoords.size() * sizeof(texCoords[0]));
		meshComp->geometry_->AppendVertexBuffer(vbBuffer);
	}

	std::vector<uint32> indices { 0, 1, 2, 0, 2, 3 };
	meshComp->geometry_->GetIndexBuffer()->indexType = IndexType::UINT32;
	meshComp->geometry_->GetIndexBuffer()->InitData(indices.data(), indices.size() * sizeof(indices[0]));

	meshComp->SetupRenderObject();

	meshComponent_ = meshComp;
}

void PostProcessBasePass::Execute(RHI::CommandEncoder* cmdEncoder)
{
	RHI::RenderPassDescriptor renderPassDescriptor;

	for (const auto& tp : attachments_)
	{
		RHI::RenderPassColorAttachmentDescriptor descriptor;
		{
			descriptor.attachment = tp.second;
			descriptor.resolveTarget = nullptr;
			descriptor.loadValue = { 0.0f, 0.0f, 0.0f, 1.0f };
			descriptor.loadOp = RHI::LoadOp::UNDEFINED;
			descriptor.storeOp = RHI::StoreOp::STORE;
		}
		renderPassDescriptor.colorAttachments.push_back(descriptor);
	}

	auto renderPassEncoder = cmdEncoder->BeginRenderPass(renderPassDescriptor);

	const RHI::Extent2D extent = { 1280, 800 };
	renderPassEncoder->SetViewport(0, 0, extent.width, extent.height, 0, 1);
	renderPassEncoder->SetScissorRect(0, 0, extent.width, extent.height);

	meshComponent_->GetRenderObject()->Render(this, ETechniqueType::TShading, ERenderSet::ERenderSet_PostProcess, *renderPassEncoder);

	renderPassEncoder->EndPass();
}

void ScreenResolvePass::Execute(RHI::CommandEncoder* cmdEncoder)
{
	const auto* texture = inputPass_->GetColorAttachments()[0].second->GetTexture();
	meshComponent_->GetRenderObject()->materialObject->SetTexture("tAlbedo", texture);

	PostProcessBasePass::Execute(cmdEncoder);
}

NS_RX_END
