#include "RenderScene.h"
#include "Engine/Engine.h"
#include "World/Camera.h"
#include "World/World.h"
#include "RHI/RHI.h"
#include "Base/FileSystem.h"
#include "Backend/Vulkan/ShaderHelper.h"

#include <chrono>

NS_RX_BEGIN

RenderScene::RenderScene()
{
	graphicPipeline_ = new GraphicPipeline();
}

RenderScene::~RenderScene()
{
}

void RenderScene::AddPrimitive(MeshComponent* mesh)
{
	if (!mesh)
	{
		return;
	}
	Assert(false, "");
}

void RenderScene::Tick(float dt)
{
	gobalRenderParams_.cameraWorldPosition = Engine::GetWorld()->GetCameraPosition();
	gobalRenderParams_.sunLightDirection = TVector3(1, -1, -1);
	gobalRenderParams_.sunLightColor = TVector4(1.0f, 1.0f, 1.0f, 1.0f);
	gobalRenderParams_.viewMatrix = Engine::GetWorld()->GetCamera()->GetViewMatrix();
	gobalRenderParams_.projMatrix = Engine::GetWorld()->GetCamera()->GetProjectionMatrix();
	gobalRenderParams_.viewProjMatrix = Engine::GetWorld()->GetCamera()->GetViewProjectionMatrix();
	gobalRenderParams_.shadowViewProjMatrix = Engine::GetWorld()->GetShadowMapCamera()->GetViewProjectionMatrix();
}

void RenderScene::SubmitGPU()
{
	graphicPipeline_->Execute(renderObjects_);
	renderObjects_.clear();
}

NS_RX_END