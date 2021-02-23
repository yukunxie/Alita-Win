#pragma once

#include "Base/Macros.h"
#include "Graphics/Pass.h"
#include "RHI.h"

NS_RX_BEGIN

class GraphicPipeline
{
public:
	GraphicPipeline();

	virtual ~GraphicPipeline()
	{
	}
public:
	void Execute();

private:
	OpaquePass opaquePass_;

	// RHI
	RHI::SwapChain* rhiSwapChain_ = nullptr;
};

NS_RX_END

