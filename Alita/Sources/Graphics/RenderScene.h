#pragma once

#include "RHI/RHI.h"

#include "Base/Macros.h"
#include "World/MeshComponent.h"
#include "GraphicPipeline.h"
#include "RenderObject.h"

NS_RX_BEGIN

struct GobalRenderParams
{
	TVector3 cameraWorldPosition;
	TVector3 sunLightDirection;
	TVector4 sunLightColor;
	TMat4x4  viewMatrix;
	TMat4x4  projMatrix;
};

class RenderScene
{
public:
	RenderScene();
	virtual ~RenderScene();

public:
	void AddPrimitive(MeshComponent* mesh);

	void AddRenderObject(RenderObject* obj)
	{
		renderObjects_.push_back(obj);
	}

	const GobalRenderParams& GetGobalRenderParams() const
	{
		return gobalRenderParams_;
	}

	void Tick(float dt);

	void SubmitGPU();

protected:

	GobalRenderParams gobalRenderParams_;

	std::vector<RenderObject*> renderObjects_;

    GraphicPipeline* graphicPipeline_ = nullptr;
};

NS_RX_END