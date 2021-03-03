#pragma once

#include "Base/Macros.h"
#include "Graphics/Pass.h"
#include "Types/Types.h"

#include "RHI.h"

NS_RX_BEGIN

class MeshComponent;
class RenderObject;

class GraphicPipeline
{
public:
	GraphicPipeline();

	virtual ~GraphicPipeline()
	{
	}

	const TExtent2D& GetWindowSize() const
	{
		return windowSize_;
	}

public:
	void Execute(const std::vector<RenderObject*>& renderObjects);

private:
	IgniterPass igniterPass_;
	OpaquePass opaquePass_;
	TExtent2D  windowSize_;

	// RHI
	RHI::SwapChain* rhiSwapChain_ = nullptr;
	RHI::Queue* graphicQueue_ = nullptr;
	RHI::CommandEncoder* rhiCommandEncoder_ = nullptr;
	
	RHI::Texture* rhiDSTexture_ = nullptr;
	RHI::TextureView* rhiDSTextureView_ = nullptr;
};

NS_RX_END

