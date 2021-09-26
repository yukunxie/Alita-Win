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

	for (const auto& tp : attachments_)
	{
		RHI::RenderPassColorAttachmentDescriptor descriptor;
		{
			descriptor.attachment = tp.RenderTarget;
			descriptor.resolveTarget = nullptr;
			descriptor.loadValue = tp.ClearColor;
			descriptor.loadOp = tp.Clear? RHI::LoadOp::CLEAR : RHI::LoadOp::LOAD;
			descriptor.storeOp = RHI::StoreOp::STORE;
		}
		renderPassDescriptor.colorAttachments.push_back(descriptor);
	}

	if (DepthStencilAttachment_.RenderTarget)
	{
		renderPassDescriptor.depthStencilAttachment = {
		.attachment = DepthStencilAttachment_.RenderTarget,
		.depthLoadOp = DepthStencilAttachment_.Clear? RHI::LoadOp::CLEAR : RHI::LoadOp::LOAD,
		.depthStoreOp = RHI::StoreOp::STORE,
		.depthLoadValue = DepthStencilAttachment_.ClearDepth,
		.stencilLoadOp = DepthStencilAttachment_.Clear ? RHI::LoadOp::CLEAR : RHI::LoadOp::LOAD,
		.stencilStoreOp = RHI::StoreOp::STORE,
		.stencilLoadValue = DepthStencilAttachment_.ClearStencil,
		};
	}

	RHI_ASSERT(RenderPassEncoder_ == nullptr);
	RenderPassEncoder_ = CommandEncoder_->BeginRenderPass(renderPassDescriptor);
}

void Pass::EndPass()
{
	RHI_ASSERT(RenderPassEncoder_);
	RenderPassEncoder_->EndPass();
	RenderPassEncoder_ = nullptr;
}

void IgniterPass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects)
{
}

ShadowMapGenPass::ShadowMapGenPass()
{
	RHI::TextureDescriptor descriptor;
	{
		descriptor.sampleCount = 1;
		descriptor.format = RHI::TextureFormat::DEPTH32FLOAT;
		descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT | RHI::TextureUsage::SAMPLED;
		descriptor.size = { shadowMapSize_.width, shadowMapSize_.height };
		descriptor.arrayLayerCount = 1;
		descriptor.mipLevelCount = 1;
		descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
	};
	dsTexture_ = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView({});
}

void ShadowMapGenPass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects)
{
	SetupDepthStencilAttachemnt(dsTexture_, true, 1.0f, 0);

	BeginPass();

	for (const auto ro : renderObjects)
	{
		ro->Render(this, ETechniqueType::TShadowmapGen, ERenderSet::ERenderSet_Opaque, *RenderPassEncoder_);
	}

	EndPass();
}

GBufferPass::GBufferPass()
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
		descriptor.format = RHI::TextureFormat::RGBA32FLOAT;
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

void GBufferPass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects)
{
	SetupOutputAttachment(0, GBuffers_.GDiffuse, true, { 0.0f, 0.0f, 0.0f, 1.0f });
	SetupOutputAttachment(1, GBuffers_.GNormal, true, { 0.0f, 0.0f, 0.0f, 1.0f });
	SetupOutputAttachment(2, GBuffers_.GPosition, true, { 0.0f, 0.0f, 0.0f, 1.0f });
	SetupOutputAttachment(3, GBuffers_.GMaterial, true, { 0.0f, 0.0f, 0.0f, 1.0f });
	SetupDepthStencilAttachemnt(dsTexture_, true, 1.0f, 0);

	BeginPass();

	for (const auto ro : renderObjects)
	{
		ro->Render(this, ETechniqueType::TGBufferGen, ERenderSet::ERenderSet_Opaque, *RenderPassEncoder_);
	}

	EndPass();
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

void SkyBoxPass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects)
{
	if (GetColorAttachments().empty())
	{
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

void FullScreenPass::Execute(RHI::CommandEncoder* cmdEncoder)
{
	BeginPass();

	meshComponent_->GetRenderObject()->Render(this, ETechniqueType::TShading, ERenderSet::ERenderSet_PostProcess, *RenderPassEncoder_);

	EndPass();
}

DeferredPass::DeferredPass()
	: FullScreenPass("Materials/DeferredLighting.json", ETechniqueType::TShading)
{
	const RHI::Extent2D extent = { 1280, 800 };

	RHI::TextureDescriptor descriptor;
	descriptor.sampleCount = 1;
	descriptor.format = RHI::TextureFormat::RGBA16FLOAT;
	descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
	descriptor.size = extent;
	descriptor.arrayLayerCount = 1;
	descriptor.mipLevelCount = 1;
	descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
	rtColor_ = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView({});

}

void DeferredPass::Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects)
{
	GBufferPass_.Reset();
	GBufferPass_.Execute(cmdEncoder, renderObjects);

	SetupOutputAttachment(0, rtColor_);
	SetupDepthStencilAttachemnt(GBufferPass_.GetDSAttachment(), false);

	auto material = meshComponent_->GetRenderObject()->MaterialObject;
	material->SetTexture("tGDiffuse", GBufferPass_.GetGBuffers().GDiffuse->GetTexture());
	material->SetTexture("tGNormal", GBufferPass_.GetGBuffers().GNormal->GetTexture());
	material->SetTexture("tGPosition", GBufferPass_.GetGBuffers().GPosition->GetTexture());
	material->SetTexture("tGMaterial", GBufferPass_.GetGBuffers().GMaterial->GetTexture());
	material->SetTexture("tShadowMap", shadowMapPass_->GetDSAttachment()->GetTexture());

	FullScreenPass::Execute(cmdEncoder);
}

void ScreenResolvePass::Execute(RHI::CommandEncoder* cmdEncoder)
{
	const auto* texture = inputPass_->GetColorAttachments()[0].RenderTarget->GetTexture();
	meshComponent_->GetRenderObject()->MaterialObject->SetTexture("tAlbedo", texture);
	FullScreenPass::Execute(cmdEncoder);
}

NS_RX_END
