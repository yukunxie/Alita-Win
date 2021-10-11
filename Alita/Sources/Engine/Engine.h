//
// Created by realxie on 2019-11-03.
//

#ifndef ALITA_ENGINE_H
#define ALITA_ENGINE_H

#include "GFX/GFX.h"
#include "Base/Macros.h"
#include "Types/Types.h"

#include <functional>
#include <map>

NS_RX_BEGIN

typedef std::function<void(std::uint32_t, float, void*)> FuncTypeEngineUpdateCallback;

struct SchedulerEntity
{
	FuncTypeEngineUpdateCallback callback;
	void* data = nullptr;
};

class World;

class RenderScene;

class EventSystem;

class Engine
{
protected:
	Engine(void* windowHandler);

	bool Init();

public:
	virtual ~Engine();

	static Engine* CreateEngine(void* data);

	static Engine* GetEngine();

public:
	
	void* GetWindowHandler()
	{
		return windowHandler_;
	}

	static RHI::Device* GetGPUDevice()
	{
		return Engine::GetEngine()->gpuDevice_;
	}

	static RenderScene* GetRenderScene()
	{
		return Engine::GetEngine()->renderScene_;
	}

	RHI::SwapChain* GetSwapchain()
	{
		return rhiSwapChain_;
	}

	static World* GetWorld()
	{
		return Engine::GetEngine()->world_;
	}

	TExtent2D GetWindowSize() const;

	void SetWindowResized(int width, int height);

public:
	void Update(float dt);

	void RunWithWorld(World* world);

	std::uint32_t StartScheduler(FuncTypeEngineUpdateCallback callback, void* data);

	void CancelScheduler(std::uint32_t schedulerId);

protected:
	static Engine* engine_;
	void* windowHandler_ = nullptr;
	World* world_ = nullptr;
	EventSystem* eventSystem_ = nullptr;
	RenderScene* renderScene_ = nullptr;

	std::uint32_t schedulerIDRecorder_ = 0;
	std::map<std::uint32_t, SchedulerEntity> schedulers_;

	RHI::Device* gpuDevice_ = nullptr;

	RHI::CommandEncoder* rhiCommandEncoder_ = nullptr;
	RHI::SwapChain* rhiSwapChain_ = nullptr;

	int  WindowWidth_ = 0;
	int  WindowHeight_ = 0;
};

NS_RX_END


#endif //ALITA_ENGINE_H
