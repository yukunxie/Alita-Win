//
// Created by realxie on 2019-10-29.
//

#include "Bloom.h"
#include "World/MeshComponent.h"
#include "World/World.h"
#include "World/Camera.h"
#include "Engine/Engine.h"
#include "Graphics/RenderScene.h"

NS_RX_BEGIN

void BloomPass::Setup(const Pass* inputPass)
{
	InputPass_ = inputPass;
	auto inputTarget = inputPass->GetColorAttachments()[0].RenderTarget;
	RTColor_->ResizeTarget(inputTarget->GetWidth(), inputTarget->GetHeight(), inputTarget->GetFormat());
}

void BloomPass::Execute()
{
	{
		DownSamplePass_.Reset();
		DownSamplePass_.Setup(InputPass_);
		DownSamplePass_.Execute();
	}

	{
		BloomBrightPass_.Reset();
		BloomBrightPass_.Setup(&DownSamplePass_);
		BloomBrightPass_.Execute();
	}

	{
		GaussianBlurPass_.Reset();
		GaussianBlurPass_.Setup(&BloomBrightPass_);
		GaussianBlurPass_.Execute();
	}

	auto extent = InputPass_->GetColorAttachments()[0].RenderTarget->GetExtent();
	auto format = InputPass_->GetColorAttachments()[0].RenderTarget->GetFormat();
	RTColor_->ResizeTarget(extent.width, extent.height, format);

	SetupOutputAttachment(0, RTColor_);

	{
		const auto* texture = InputPass_->GetColorAttachments()[0].RenderTarget->GetTexture();
		SetTexture("tAlbedo", texture);
	}

	{
		const auto* texture = GaussianBlurPass_.GetColorAttachments()[0].RenderTarget->GetTexture();
		SetTexture("tBloomMap", texture);
	}

	//SetFloat("BlurDirection", 0, 4, &Param.x);

	FullScreenPass::Execute();
	
}

NS_RX_END
