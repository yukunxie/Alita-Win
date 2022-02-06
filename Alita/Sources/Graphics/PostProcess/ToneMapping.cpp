//
// Created by realxie on 2019-10-29.
//

#include "ToneMapping.h"
#include "World/MeshComponent.h"
#include "World/World.h"
#include "World/Camera.h"
#include "Engine/Engine.h"
#include "Graphics/RenderScene.h"

NS_RX_BEGIN

void ToneMapping::Setup(const Pass* inputPass)
{
	InputPass_ = inputPass;
	auto inputTarget = inputPass->GetColorAttachments()[0].RenderTarget;
	RTColor_->ResizeTarget(inputTarget->GetWidth(), inputTarget->GetHeight(), gfx::TextureFormat::RGBA8UNORM);
}

void ToneMapping::Execute()
{
	auto extent = InputPass_->GetColorAttachments()[0].RenderTarget->GetExtent();
	auto format = InputPass_->GetColorAttachments()[0].RenderTarget->GetFormat();
	RTColor_->ResizeTarget(extent.width, extent.height);

	SetupOutputAttachment(0, RTColor_);

	{
		auto texture = InputPass_->GetColorAttachments()[0].RenderTarget->GetTexture();
		SetTexture("tAlbedo", texture);
	}

	//SetFloat("BlurDirection", 0, 4, &Param.x);

	FullScreenPass::Execute();
	
}

NS_RX_END
