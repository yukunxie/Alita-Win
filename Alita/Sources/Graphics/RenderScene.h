#pragma once

#include "RHI.h"

#include "Base/Macros.h"
#include "World/MeshComponent.h"

NS_RX_BEGIN

class RenderScene
{
public:
	RenderScene();
	virtual ~RenderScene();

public:
	void AddPrimitive(MeshComponent* mesh);

	void SubmitGPU();

protected:
	RHI::Buffer* rhiVertexBuffer_ = nullptr;
	RHI::Buffer* rhiIndexBuffer_ = nullptr;
	RHI::CommandEncoder* rhiCommandEncoder_ = nullptr;
};

NS_RX_END