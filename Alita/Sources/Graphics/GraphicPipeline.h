#pragma once

#include "Base/Macros.h"
#include "Graphics/Pass.h"
#include "Types/Types.h"
#include "PostProcess/Bloom.h"
#include "PostProcess/ToneMapping.h"

#include "GFX/GFX.h"

NS_RX_BEGIN

class MeshComponent;
class RenderObject;

class GraphicPipeline
{
public:
	GraphicPipeline();

	virtual ~GraphicPipeline();

	const TExtent2D GetWindowSize() const
	{
		auto extent = RHISwapChain_->GetCurrentTexture()->GetTextureSize();
		return { extent.width, extent.height };
	}

	float GetRenderTargetScaleFactor() const
	{
		return 1.0;
	}

	gfx::CommandEncoderPtr GetCommandEncoder() { return CommandEncoder_; }

	gfx::SwapChainPtr GetSwapChain() 
	{ 
		return RHISwapChain_;
	}

public:
	void Execute(const std::vector<RenderObject*>& renderObjects);

private:
	ShadowMapGenPass ShadowGenPass_;
	IgniterPass IgniterPass_;
	OpaquePass OpaquePass_;
	SkyBoxPass SkyBoxPass_;
	DeferredPass DeferredPass_;
	ScreenResolvePass ScreenResolvePass_;
	BloomPass DownSamplePass_;
	ToneMapping ToneMappingPass_;
	OutlinePass OutlinePass_;
	CloudPass   CloudPass_;
	TExtent2D  WindowSize_;

	// RHI
	gfx::SwapChainPtr RHISwapChain_ = nullptr;
	gfx::QueuePtr GraphicQueue_ = nullptr;
	gfx::CommandEncoderPtr CommandEncoder_ = nullptr;
	
	gfx::TexturePtr rhiDSTexture_ = nullptr;
	gfx::TextureViewPtr rhiDSTextureView_ = nullptr;
};

NS_RX_END

