#pragma once

#include "GFX/GFX.h"

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
	TMat4x4  viewProjMatrix;
	TMat4x4	 shadowViewProjMatrix;
};

class RenderScene
{
public:
	RenderScene();
	virtual ~RenderScene();

public:
	GraphicPipeline* GetGraphicPipeline() { return GraphicPipeline_; }

	void AddPrimitive(MeshComponent* mesh);

	void AddRenderObject(RenderObject* obj)
	{
		RenderObjects_.push_back(obj);
	}

	const GobalRenderParams& GetGobalRenderParams() const
	{
		return GobalRenderParams_;
	}

	void Tick(float dt);

	void SubmitGPU();

protected:

	GobalRenderParams GobalRenderParams_;

	std::vector<RenderObject*> RenderObjects_;

    GraphicPipeline* GraphicPipeline_ = nullptr;
};

NS_RX_END