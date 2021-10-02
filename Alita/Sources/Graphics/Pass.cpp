//
// Created by realxie on 2019-10-29.
//

#include "Pass.h"
#include "World/MeshComponent.h"
#include "World/World.h"
#include "World/Camera.h"
#include "RenderObject.h"
#include "Engine/Engine.h"
#include "Graphics/RenderScene.h"

NS_RX_BEGIN

void Pass::BeginPass()
{
	CommandEncoder_ = Engine::GetRenderScene()->GetGraphicPipeline()->GetCommandEncoder();

	std::sort(attachments_.begin(), attachments_.end(), [](const AttachmentConfig& a, const AttachmentConfig& b) {return a.Slot < b.Slot; });

	RHI::RenderPassDescriptor renderPassDescriptor;

	RHI::Extent3D extent;

	for (const auto& tp : attachments_)
	{
		RHI::RenderPassColorAttachmentDescriptor descriptor;
		{
			descriptor.attachment = tp.RenderTarget->GetTextureView();
			descriptor.resolveTarget = nullptr;
			descriptor.loadValue = tp.RenderTarget->GetClearColor();
			descriptor.loadOp = tp.Clear? RHI::LoadOp::CLEAR : RHI::LoadOp::LOAD;
			descriptor.storeOp = RHI::StoreOp::STORE;
		}
		renderPassDescriptor.colorAttachments.push_back(descriptor);
		extent = tp.RenderTarget->GetExtent();
	}

	if (DepthStencilAttachment_.RenderTarget)
	{
		renderPassDescriptor.depthStencilAttachment = {
		.attachment = DepthStencilAttachment_.RenderTarget->GetTextureView(),
		.depthLoadOp = DepthStencilAttachment_.Clear? RHI::LoadOp::CLEAR : RHI::LoadOp::LOAD,
		.depthStoreOp = RHI::StoreOp::STORE,
		.depthLoadValue = DepthStencilAttachment_.RenderTarget->GetClearDepth(),
		.stencilLoadOp = DepthStencilAttachment_.Clear ? RHI::LoadOp::CLEAR : RHI::LoadOp::LOAD,
		.stencilStoreOp = RHI::StoreOp::STORE,
		.stencilLoadValue = DepthStencilAttachment_.RenderTarget->GetClearStencil(),
		};
		extent = DepthStencilAttachment_.RenderTarget->GetExtent();
	}

	RHI_ASSERT(RenderPassEncoder_ == nullptr);
	RenderPassEncoder_ = CommandEncoder_->BeginRenderPass(renderPassDescriptor);
	{
		RenderPassEncoder_->SetViewport(0, 0, extent.width, extent.height, 0, 1);
		RenderPassEncoder_->SetScissorRect(0, 0, extent.width, extent.height);
		RenderPassEncoder_->SetDepthBias(0, 0, 0);
	}
}

void Pass::EndPass()
{
	RHI_ASSERT(RenderPassEncoder_);
	RenderPassEncoder_->EndPass();
	RenderPassEncoder_ = nullptr;
}

void IgniterPass::Execute(const std::vector<RenderObject*>& renderObjects)
{
}

ShadowMapGenPass::ShadowMapGenPass()
{
	dsTexture_ = std::make_shared<RenderTarget>(shadowMapSize_.width, shadowMapSize_.height, RHI::TextureFormat::DEPTH32FLOAT);
}

void ShadowMapGenPass::Execute(const std::vector<RenderObject*>& renderObjects)
{
	SetupDepthStencilAttachemnt(dsTexture_, true);

	BeginPass();

	RenderPassEncoder_->SetDepthBias(1.25f, 0, 1.75f);

	for (const auto ro : renderObjects)
	{
		ro->Render(this, ETechniqueType::TShadowMapGen, ERenderSet::ERenderSet_Opaque, *RenderPassEncoder_);
	}

	EndPass();
}

GBufferPass::GBufferPass()
{
	const RHI::Extent2D extent = { 1280, 800 };
	GBuffers_.GDiffuse = std::make_shared<RenderTarget>(extent.width, extent.height, RHI::TextureFormat::RGBA16FLOAT);
	GBuffers_.GEmissive = std::make_shared<RenderTarget>(extent.width, extent.height, RHI::TextureFormat::RGBA16FLOAT);
	GBuffers_.GNormal = std::make_shared<RenderTarget>(extent.width, extent.height, RHI::TextureFormat::RGBA16FLOAT);
	GBuffers_.GPosition = std::make_shared<RenderTarget>(extent.width, extent.height, RHI::TextureFormat::RGBA32FLOAT);
	GBuffers_.GMaterial = std::make_shared<RenderTarget>(extent.width, extent.height, RHI::TextureFormat::RGBA16FLOAT);
	dsTexture_ = std::make_shared<RenderTarget>(extent.width, extent.height, RHI::TextureFormat::DEPTH24PLUS_STENCIL8);
}

void GBufferPass::Execute(const std::vector<RenderObject*>& renderObjects)
{
	auto wsize = Engine::GetRenderScene()->GetGraphicPipeline()->GetWindowSize();
	GBuffers_.GDiffuse->ResizeTarget(wsize.width, wsize.height);
	GBuffers_.GEmissive->ResizeTarget(wsize.width, wsize.height);
	GBuffers_.GNormal->ResizeTarget(wsize.width, wsize.height);
	GBuffers_.GPosition->ResizeTarget(wsize.width, wsize.height);
	GBuffers_.GMaterial->ResizeTarget(wsize.width, wsize.height);
	dsTexture_->ResizeTarget(wsize.width, wsize.height);

	SetupOutputAttachment(0, GBuffers_.GDiffuse, true);
	SetupOutputAttachment(1, GBuffers_.GEmissive, true);
	SetupOutputAttachment(2, GBuffers_.GNormal, true);
	SetupOutputAttachment(3, GBuffers_.GPosition, true);
	SetupOutputAttachment(4, GBuffers_.GMaterial, true);
	SetupDepthStencilAttachemnt(dsTexture_, true);

	BeginPass();

	for (const auto ro : renderObjects)
	{
		ro->Render(this, ETechniqueType::TGBufferGen, ERenderSet::ERenderSet_Opaque, *RenderPassEncoder_);
	}

	EndPass();
}

OpaquePass::OpaquePass()
{
	rtColor_ = std::make_shared<RenderTarget>(RHI::TextureFormat::RGBA16FLOAT);
	rtDepthStencil_ = std::make_shared<RenderTarget>(RHI::TextureFormat::DEPTH24PLUS_STENCIL8);
}

void OpaquePass::Execute(const std::vector<RenderObject*>& renderObjects)
{
	auto wsize = Engine::GetRenderScene()->GetGraphicPipeline()->GetWindowSize();
	rtColor_->ResizeTarget(wsize.width, wsize.height);
	rtDepthStencil_->ResizeTarget(wsize.width, wsize.height);

	SetupOutputAttachment(0, rtColor_);
	SetupDepthStencilAttachemnt(rtDepthStencil_);

	BeginPass();

	for (const auto ro : renderObjects)
	{
		ro->Render(this, ETechniqueType::TShading, ERenderSet::ERenderSet_Opaque, *RenderPassEncoder_);
	}

	EndPass();
}

SkyBoxPass::SkyBoxPass()
{
	rtColor_ = std::make_shared<RenderTarget>(RHI::TextureFormat::RGBA16FLOAT);
	rtDepthStencil_ = std::make_shared<RenderTarget>(RHI::TextureFormat::DEPTH24PLUS_STENCIL8);
}

void SkyBoxPass::Setup(const Pass* mainPass, const Pass* depthPass)
{
	if (mainPass)
	{
		SetupOutputAttachment(0, mainPass->GetColorAttachments()[0].RenderTarget, false);
	}
	if (depthPass && depthPass->GetDSAttachment())
	{
		SetupDepthStencilAttachemnt(depthPass->GetDSAttachment(), false);
	}
	else if (mainPass->GetDSAttachment())
	{
		SetupDepthStencilAttachemnt(mainPass->GetDSAttachment(), false);
	}
}

void SkyBoxPass::Execute(const std::vector<RenderObject*>& renderObjects)
{
	if (GetColorAttachments().empty())
	{
		auto wsize = Engine::GetRenderScene()->GetGraphicPipeline()->GetWindowSize();
		rtColor_->ResizeTarget(wsize.width, wsize.height);
		SetupOutputAttachment(0, rtColor_);
	}

	BeginPass();

	for (const auto ro : renderObjects)
	{
		ro->Render(this, ETechniqueType::TSkyBox, ERenderSet::ERenderSet_SkyBox, *RenderPassEncoder_);
	}

	EndPass();
}

FullScreenPass::FullScreenPass(const std::string& materialName, ETechniqueType technique)
{
	MeshComponent* meshComp = new MeshComponent();
	meshComp->Geometry_ = new Geometry;
	meshComp->Material_ = new Material(materialName);

	{
		std::vector<float> vertices = {-1, -1, 0,	// bottom left corner
							-1,  1, 0,	// top left corner
							 1,  1, 0,	// top right corner
							 1, -1, 0 };// bottom right corner

		auto vbBuffer = new VertexBuffer();
		vbBuffer->kind = VertexBufferAttriKind::POSITION;
		vbBuffer->format = InputAttributeFormat::FLOAT3;
		vbBuffer->InitData(vertices.data(), vertices.size() * sizeof(vertices[0]));
		meshComp->Geometry_->AppendVertexBuffer(vbBuffer);
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
		meshComp->Geometry_->AppendVertexBuffer(vbBuffer);
	}

	std::vector<uint32> indices { 0, 1, 2, 0, 2, 3 };
	meshComp->Geometry_->GetIndexBuffer()->indexType = IndexType::UINT32;
	meshComp->Geometry_->GetIndexBuffer()->InitData(indices.data(), indices.size() * sizeof(indices[0]));

	meshComp->SetupRenderObject();

	meshComponent_ = meshComp;
}

void FullScreenPass::Execute()
{
	BeginPass();

	meshComponent_->GetRenderObject()->Render(this, ETechniqueType::TShading, ERenderSet::ERenderSet_PostProcess, *RenderPassEncoder_);

	EndPass();
}

DeferredPass::DeferredPass()
	: FullScreenPass("Materials/DeferredLighting.json", ETechniqueType::TShading)
{
	rtColor_ = std::make_shared<RenderTarget>(RHI::TextureFormat::RGBA16FLOAT);
}

void DeferredPass::Execute(const std::vector<RenderObject*>& renderObjects)
{
	GBufferPass_.Reset();
	GBufferPass_.Execute(renderObjects);

	auto wsize = Engine::GetRenderScene()->GetGraphicPipeline()->GetWindowSize();
	rtColor_->ResizeTarget(wsize.width, wsize.height);

	SetupOutputAttachment(0, rtColor_);
	SetupDepthStencilAttachemnt(GBufferPass_.GetDSAttachment(), false);

	auto material = meshComponent_->GetRenderObject()->MaterialObject;
	material->SetTexture("tGDiffuse", GBufferPass_.GetGBuffers().GDiffuse->GetTexture());
	material->SetTexture("tGEmissive", GBufferPass_.GetGBuffers().GEmissive->GetTexture());
	material->SetTexture("tGNormal", GBufferPass_.GetGBuffers().GNormal->GetTexture());
	material->SetTexture("tGPosition", GBufferPass_.GetGBuffers().GPosition->GetTexture());
	material->SetTexture("tGMaterial", GBufferPass_.GetGBuffers().GMaterial->GetTexture());
	material->SetTexture("tShadowMap", shadowMapPass_->GetDSAttachment()->GetTexture());

	FullScreenPass::Execute();
}

void ScreenResolvePass::Execute()
{
	RTSwapChain_->Reset();
	SetupOutputAttachment(0, RTSwapChain_);

	const auto* texture = inputPass_->GetColorAttachments()[0].RenderTarget->GetTexture();
	meshComponent_->GetRenderObject()->MaterialObject->SetTexture("tAlbedo", texture);
	FullScreenPass::Execute();
	/*CommandEncoder_ = Engine::GetRenderScene()->GetGraphicPipeline()->GetCommandEncoder();

	auto swapchain = Engine::GetRenderScene()->GetGraphicPipeline()->GetSwapChain();
	RHI::TextureView* colorAttachment = swapchain->GetCurrentTexture()->CreateView({});

	RHI::RenderPassDescriptor renderPassDescriptor;

	RHI::RenderPassColorAttachmentDescriptor descriptor;
	{
		descriptor.attachment = colorAttachment;
		descriptor.resolveTarget = nullptr;
		descriptor.loadValue = { 0, 0, 0, 1 };
		descriptor.loadOp =  RHI::LoadOp::UNDEFINED;
		descriptor.storeOp = RHI::StoreOp::STORE;
	}
	renderPassDescriptor.colorAttachments.push_back(descriptor);
	auto extent = colorAttachment->GetTexture()->GetTextureSize();

	RenderPassEncoder_ = CommandEncoder_->BeginRenderPass(renderPassDescriptor);
	{
		RenderPassEncoder_->SetViewport(0, 0, extent.width, extent.height, 0, 1);
		RenderPassEncoder_->SetScissorRect(0, 0, extent.width, extent.height);
		RenderPassEncoder_->SetDepthBias(0, 0, 0);
	}

	const auto* texture = inputPass_->GetColorAttachments()[0].RenderTarget->GetTexture();
	meshComponent_->GetRenderObject()->MaterialObject->SetTexture("tAlbedo", texture);
	meshComponent_->GetRenderObject()->Render(this, ETechniqueType::TShading, ERenderSet::ERenderSet_PostProcess, *RenderPassEncoder_);

	RenderPassEncoder_->EndPass();*/
}

NS_RX_END
