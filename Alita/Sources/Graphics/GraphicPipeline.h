#pragma once

#include "Base/Macros.h"
#include "Graphics/Pass.h"
#include "Types/Types.h"

#include "RHI/RHI.h"

NS_RX_BEGIN

class MeshComponent;
class RenderObject;

class GraphicPipeline
{
public:
	GraphicPipeline();

	virtual ~GraphicPipeline();

	const TExtent2D& GetWindowSize() const
	{
		return WindowSize_;
	}

	RHI::CommandEncoder* GetCommandEncoder() { return CommandEncoder_; }

public:
	void Execute(const std::vector<RenderObject*>& renderObjects);

private:
	ShadowMapGenPass ShadowGenPass_;
	IgniterPass IgniterPass_;
	OpaquePass OpaquePass_;
	SkyBoxPass SkyBoxPass_;
	DeferredPass DeferredPass_;
	ScreenResolvePass ScreenResolvePass_;
	TExtent2D  WindowSize_;

	// RHI
	RHI::SwapChain* RHISwapChain_ = nullptr;
	RHI::Queue* GraphicQueue_ = nullptr;
	RHI::CommandEncoder* CommandEncoder_ = nullptr;
	
	RHI::Texture* rhiDSTexture_ = nullptr;
	RHI::TextureView* rhiDSTextureView_ = nullptr;
};

NS_RX_END

