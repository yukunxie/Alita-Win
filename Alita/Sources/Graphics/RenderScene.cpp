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
	GraphicPipeline_ = new GraphicPipeline();
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
	GobalRenderParams_.cameraWorldPosition = Engine::GetWorld()->GetCameraPosition();
	GobalRenderParams_.sunLightDirection = TVector3(-10, -20, -20);
	GobalRenderParams_.sunLightColor = TVector4(1.0f, 1.0f, 1.0f, 1.0f);
	GobalRenderParams_.viewMatrix = Engine::GetWorld()->GetCamera()->GetViewMatrix();
	GobalRenderParams_.projMatrix = Engine::GetWorld()->GetCamera()->GetProjectionMatrix();
	GobalRenderParams_.viewProjMatrix = Engine::GetWorld()->GetCamera()->GetViewProjectionMatrix();

	float near_plane = 0.1f, far_plane = 1000.f;
	glm::mat4 lightOrthoPoojection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(-GobalRenderParams_.sunLightDirection, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 lightSpaceMatrix = lightOrthoPoojection * lightView;
	GobalRenderParams_.shadowViewProjMatrix = lightSpaceMatrix;
}

void RenderScene::SubmitGPU()
{
	GraphicPipeline_->Execute(RenderObjects_);
	RenderObjects_.clear();
}

NS_RX_END