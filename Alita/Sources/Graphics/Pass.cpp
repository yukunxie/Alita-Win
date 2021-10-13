//
// Created by realxie on 2019-10-29.
//

#include "Pass.h"
#include "World/MeshComponent.h"
#include "World/World.h"
#include "World/Camera.h"
#include "RenderObject.h"
#include "Engine/Engine.h"
#include "RenderScene.h"
#include "Material.h"

NS_RX_BEGIN

void Pass::BeginPass()
{
	CommandEncoder_ = Engine::GetRenderScene()->GetGraphicPipeline()->GetCommandEncoder();

	CommandEncoder_->PushDebugGroup(PassName_);

	std::sort(attachments_.begin(), attachments_.end(), [](const AttachmentConfig& a, const AttachmentConfig& b) {return a.Slot < b.Slot; });

	gfx::RenderPassDescriptor renderPassDescriptor;

	gfx::Extent3D extent;

	for (const auto& tp : attachments_)
	{
		gfx::RenderPassColorAttachmentDescriptor descriptor;
		{
			descriptor.attachment = tp.RenderTarget->GetTextureView();
			descriptor.resolveTarget = nullptr;
			descriptor.loadValue = tp.RenderTarget->GetClearColor();
			descriptor.loadOp = tp.Clear? gfx::LoadOp::CLEAR : gfx::LoadOp::LOAD;
			descriptor.storeOp = gfx::StoreOp::STORE;
		}
		renderPassDescriptor.colorAttachments.push_back(descriptor);
		extent = tp.RenderTarget->GetExtent();
	}

	if (DepthStencilAttachment_.RenderTarget)
	{
		renderPassDescriptor.depthStencilAttachment = {
		.attachment = DepthStencilAttachment_.RenderTarget->GetTextureView(),
		.depthLoadOp = DepthStencilAttachment_.Clear? gfx::LoadOp::CLEAR : gfx::LoadOp::LOAD,
		.depthStoreOp = gfx::StoreOp::STORE,
		.depthLoadValue = DepthStencilAttachment_.RenderTarget->GetClearDepth(),
		.stencilLoadOp = DepthStencilAttachment_.Clear ? gfx::LoadOp::CLEAR : gfx::LoadOp::LOAD,
		.stencilStoreOp = gfx::StoreOp::STORE,
		.stencilLoadValue = DepthStencilAttachment_.RenderTarget->GetClearStencil(),
		};
		extent = DepthStencilAttachment_.RenderTarget->GetExtent();
	}

	GFX_ASSERT(RenderPassEncoder_ == nullptr);
	RenderPassEncoder_ = CommandEncoder_->BeginRenderPass(renderPassDescriptor);
	{
		RenderPassEncoder_->SetViewport(0, 0, extent.width, extent.height, 0, 1);
		RenderPassEncoder_->SetScissorRect(0, 0, extent.width, extent.height);
		RenderPassEncoder_->SetDepthBias(0, 0, 0);
	}
}

void Pass::EndPass()
{
	GFX_ASSERT(RenderPassEncoder_);
	RenderPassEncoder_->EndPass();
	RenderPassEncoder_ = nullptr;

	CommandEncoder_->PopDebugGroup();
}

void IgniterPass::Execute(const std::vector<RenderObject*>& renderObjects)
{
}

ShadowMapGenPass::ShadowMapGenPass()
{
	PassName_ = "ShadowMapGenPass";

	dsTexture_ = std::make_shared<RenderTarget>(shadowMapSize_.width, shadowMapSize_.height, gfx::TextureFormat::DEPTH24PLUS_STENCIL8);
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
	PassName_ = "GBufferPass";

	const gfx::Extent2D extent = { 1280, 800 };
	GBuffers_.GDiffuse = std::make_shared<RenderTarget>(extent.width, extent.height, gfx::TextureFormat::RGBA16FLOAT, "GDiffuse");
	GBuffers_.GEmissive = std::make_shared<RenderTarget>(extent.width, extent.height, gfx::TextureFormat::RGBA16FLOAT, "GEmissive");
	GBuffers_.GNormal = std::make_shared<RenderTarget>(extent.width, extent.height, gfx::TextureFormat::RGBA16FLOAT, "GNormal");
	GBuffers_.GPosition = std::make_shared<RenderTarget>(extent.width, extent.height, gfx::TextureFormat::RGBA32FLOAT, "GPosition");
	GBuffers_.GMaterial = std::make_shared<RenderTarget>(extent.width, extent.height, gfx::TextureFormat::RGBA16FLOAT, "GMaterial");
	dsTexture_ = std::make_shared<RenderTarget>(extent.width, extent.height, gfx::TextureFormat::DEPTH24PLUS_STENCIL8);
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
	PassName_ = "OpaquePass";

	rtColor_ = std::make_shared<RenderTarget>(gfx::TextureFormat::RGBA16FLOAT);
	rtDepthStencil_ = std::make_shared<RenderTarget>(gfx::TextureFormat::DEPTH24PLUS_STENCIL8);
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
	PassName_ = "SkyBoxPass";

	rtColor_ = std::make_shared<RenderTarget>(gfx::TextureFormat::RGBA16FLOAT);
	rtDepthStencil_ = std::make_shared<RenderTarget>(gfx::TextureFormat::DEPTH24PLUS_STENCIL8);
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

void FullScreenPass::SetTexture(const std::string& name, const gfx::Texture* texture)
{
	meshComponent_->GetRenderObject()->MaterialObject->SetTexture(name, texture);
}

void FullScreenPass::SetFloat(const std::string& name, std::uint32_t offset, std::uint32_t count, const float* data)
{
	meshComponent_->GetRenderObject()->MaterialObject->SetFloat(name, offset, count, data);
}

Material* FullScreenPass::GetMaterial()
{
	return meshComponent_->GetRenderObject()->MaterialObject;
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
	PassName_ = "DeferredLightingPass";

	rtColor_ = std::make_shared<RenderTarget>(gfx::TextureFormat::RGBA16FLOAT);
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
	PassName_ = "ScreenResolvePass";

	RTSwapChain_->Reset();
	SetupOutputAttachment(0, RTSwapChain_);

	const auto* texture = inputPass_->GetColorAttachments()[0].RenderTarget->GetTexture();
	meshComponent_->GetRenderObject()->MaterialObject->SetTexture("tAlbedo", texture);
	FullScreenPass::Execute();
}

DownSamplePass::DownSamplePass()
	:FullScreenPass("Materials/DownSample.json", ETechniqueType::TShading)
{
	PassName_ = "DownSamplePass";
	RTColor_ = std::make_shared<RenderTarget>();
}

void DownSamplePass::Execute()
{
	auto extent = InputPass_->GetColorAttachments()[0].RenderTarget->GetExtent();
	auto format = InputPass_->GetColorAttachments()[0].RenderTarget->GetFormat();
	RTColor_->ResizeTarget(extent.width / 2, extent.height / 2, format);

	SetupOutputAttachment(0, RTColor_);

	const auto* texture = InputPass_->GetColorAttachments()[0].RenderTarget->GetTexture();
	SetTexture("tAlbedo", texture);
	float imageSizeInfo[4] = { extent.width, extent.height, 1.0f / extent.width , 1.0f / extent.height };
	SetFloat("ImageSize", 0, 4, imageSizeInfo);

	float params[4] = { FilterType_, 0, 0 , 0 };
	SetFloat("Param", 0, 4, params);

	FullScreenPass::Execute();
}

BloomBrightPass::BloomBrightPass()
	: FullScreenPass("Materials/Bloom-Bright.json", ETechniqueType::TShading)
{
	PassName_ = "BloomBrightPass";

	RTColor_ = std::make_shared<RenderTarget>();
}

void BloomBrightPass::Execute()
{
	auto extent = InputPass_->GetColorAttachments()[0].RenderTarget->GetExtent();
	auto format = InputPass_->GetColorAttachments()[0].RenderTarget->GetFormat();
	RTColor_->ResizeTarget(extent.width, extent.height, format);

	SetupOutputAttachment(0, RTColor_);

	const auto* texture = InputPass_->GetColorAttachments()[0].RenderTarget->GetTexture();
	SetTexture("tAlbedo", texture);

	float params[4] = { 0.5f, 0, 0 , 0 };
	SetFloat("Param", 0, 4, params);

	FullScreenPass::Execute();
}

GaussianBlur::GaussianBlur()
	: FullScreenPass("Materials/GaussianBlur.json", ETechniqueType::TShading)
	, VerticalGuassianBlurPass_(new GaussianBlur(true))
{
	PassName_ = "GaussianBlur-X";

	RTColor_ = std::make_shared<RenderTarget>();
	Param = TVector4(1.0f, 0, 1.0f, 0);

	//VerticalGuassianBlurPass_ = std::make_shared< GaussianBlur>(true);
}

GaussianBlur::GaussianBlur(bool isVertical)
	: FullScreenPass("Materials/GaussianBlur.json", ETechniqueType::TShading)
{
	PassName_ = "GaussianBlur-Y";
	RTColor_ = std::make_shared<RenderTarget>();
	Param = TVector4(0, 1.0f, 1.0f, 0);
}

void GaussianBlur::Setup(const Pass* inputPass)
{
	if (VerticalGuassianBlurPass_)
	{
		VerticalGuassianBlurPass_->Setup(inputPass);
	}
	else
	{
		InputPass_ = inputPass;
	}
}

void GaussianBlur::Execute()
{
	Reset();

	if (VerticalGuassianBlurPass_)
	{
		VerticalGuassianBlurPass_->Execute();

		InputPass_ = VerticalGuassianBlurPass_.get();
	}

	auto extent = InputPass_->GetColorAttachments()[0].RenderTarget->GetExtent();
	auto format = InputPass_->GetColorAttachments()[0].RenderTarget->GetFormat();
	RTColor_->ResizeTarget(extent.width, extent.height, format);

	SetupOutputAttachment(0, RTColor_);

	const auto* texture = InputPass_->GetColorAttachments()[0].RenderTarget->GetTexture();
	SetTexture("tAlbedo", texture);

	float imageSizeInfo[4] = { extent.width, extent.height, 1.0f / extent.width , 1.0f / extent.height };
	SetFloat("ImageSize", 0, 4, imageSizeInfo);

	SetFloat("BlurDirection", 0, 4, &Param.x);

	FullScreenPass::Execute();
}


OutlineMarkPass::OutlineMarkPass()
{
	PassName_ = "OutlineMarkPass";
	rtColor_ = std::make_shared<RenderTarget>(gfx::TextureFormat::RGBA8UNORM);
	rtDepthStencil_ = std::make_shared<RenderTarget>(gfx::TextureFormat::DEPTH24PLUS_STENCIL8);
}

void OutlineMarkPass::ResizeTarget(uint32 width, uint32 height)
{
	rtColor_->ResizeTarget(width, height); 
	rtDepthStencil_->ResizeTarget(width, height);
}

void OutlineMarkPass::Execute(const std::vector<RenderObject*>& renderObjects)
{
	SetupOutputAttachment(0, rtColor_, true);
	SetupDepthStencilAttachemnt(rtDepthStencil_, true);

	BeginPass();

	for (auto ro : renderObjects)
	{
		if (ro->bSelected)
		{
			ro->Render(this, ETechniqueType::TOutline, ERenderSet::ERenderSet_Opaque, *RenderPassEncoder_);
		}
	}

	EndPass();

	
}

OutlinePass::OutlinePass()
	: FullScreenPass("Materials/Outline.json", ETechniqueType::TShading)
{
	PassName_ = "OutlinePass";
	rtColor_ = std::make_shared<RenderTarget>();
}

void OutlinePass::Setup(const Pass* inputPass)
{
	InputPass_ = inputPass;
}

void OutlinePass::Execute(const std::vector<RenderObject*>& renderObjects)
{
	CommandEncoder_ = Engine::GetRenderScene()->GetGraphicPipeline()->GetCommandEncoder();
	CommandEncoder_->PushDebugGroup("Outline");

	auto extent = InputPass_->GetColorAttachments()[0].RenderTarget->GetExtent();
	auto format = InputPass_->GetColorAttachments()[0].RenderTarget->GetFormat();

	OutlineMarkPass_.Reset();
	OutlineMarkPass_.ResizeTarget(extent.width, extent.height);
	OutlineMarkPass_.Execute(renderObjects);

	BlurPass_.Reset();
	BlurPass_.Setup(&OutlineMarkPass_);
	BlurPass_.Execute();

	
	rtColor_->ResizeTarget(extent.width, extent.height, format);

	SetupOutputAttachment(0, rtColor_, true);

	{
		const auto* texture = InputPass_->GetColorAttachments()[0].RenderTarget->GetTexture();
		SetTexture("tColorMap", texture);
	}
	
	{
		const auto* texture = OutlineMarkPass_.GetColorAttachments()[0].RenderTarget->GetTexture();
		SetTexture("tMaskMap", texture);
	}

	{
		const auto* texture = BlurPass_.GetColorAttachments()[0].RenderTarget->GetTexture();
		SetTexture("tBlurMap", texture);
	}

	FullScreenPass::Execute();

	CommandEncoder_->PopDebugGroup();
}

CloudPass::CloudPass()
{
	PassName_ = "CloudPass";
	SphereMeshComponent_ = TSharedPtr<MeshComponent>(MeshComponentBuilder::CreateSphere("Materials/Cloud.json"));
}

void CloudPass::Execute()
{
	SphereMeshComponent_->SetupRenderObject();

	SetupOutputAttachment(0, InputPass_->GetColorAttachments()[0].RenderTarget, false);
	SetupDepthStencilAttachemnt(DepthPass_->GetDSAttachment(), false);

	BeginPass();

	auto matrix = TMat4x4(1);
	matrix = glm::scale(matrix, TVector3(30000));

	SphereMeshComponent_->GetMaterial()->SetFloat("WorldMatrix", 0, 16, (float*)&matrix);

	SphereMeshComponent_->GetRenderObject()->Render(this, ETechniqueType::TVolumeCloud, ERenderSet::ERenderSet_Opaque, *RenderPassEncoder_);

	EndPass();
}


NS_RX_END
