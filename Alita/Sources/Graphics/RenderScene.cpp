#include "RenderScene.h"
#include "Engine/Engine.h"
#include "World/Camera.h"
#include "World/World.h"
#include "RHI.h"
#include "Base/FileSystem.h"
#include "Backend/Vulkan/ShaderHelper.h"

#include <chrono>

NS_RX_BEGIN

struct UniformBufferObject
{
	TMat4x4 model;
	TMat4x4 view;
	TMat4x4 proj;
};

Material* gMaterial = nullptr;
Camera* gCamera = nullptr;

RenderScene::RenderScene()
{
	graphicPipeline_ = new GraphicPipeline();

	gMaterial = new Material("Shaders/CommonMaterial.json");
	gCamera = new PerspectiveCamera(45.0, 800.0f / 1280.f, 0.1f, 100.f);
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
	meshComponents_.push_back(mesh);
}

void RenderScene::Tick(float dt)
{
	gobalRenderParams_.cameraWorldPosition = Engine::GetWorld()->GetCameraPosition();
	gobalRenderParams_.sunLightDirection = TVector3(1, -1, -1);
	gobalRenderParams_.sunLightColor = TVector4(1.0f, 1.0f, 1.0f, 1.0f);;
	gobalRenderParams_.viewMatrix = Engine::GetWorld()->GetCamera()->GetViewMatrix();
	gobalRenderParams_.projMatrix = Engine::GetWorld()->GetCamera()->GetProjectionMatrix();
}

void RenderScene::testRotate()
{
	/*static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(
		currentTime - startTime).count();

	RHI::Extent3D viewport = {1280, 800, 1};

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(.0f, 8.0f, .0f), glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), viewport.width /
		(float)viewport.height, 2.0f,
		20.0f);
	ubo.proj[1][1] *= -1;

	rhiDevice_->WriteBuffer(rhiUniformBuffer_, &ubo, 0, sizeof(UniformBufferObject));*/
}

void RenderScene::SubmitGPU()
{
	graphicPipeline_->Execute(renderObjects_);
	meshComponents_.clear();
	renderObjects_.clear();
}

NS_RX_END