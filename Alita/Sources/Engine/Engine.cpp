//
// Created by realxie on 2019-11-03.
//

#include "Engine.h"
#include "World/World.h"
#include "Base/FileSystem.h"
#include "Graphics/RenderScene.h"
#include "Event/EventSystem.h"

#include "Backend/Vulkan/ShaderHelper.h"
#include "Backend/Vulkan/VKDevice.h"

NS_RX_BEGIN

class DeviceHandler : public RHI::IDeviceExternalDeps
{
public:
    DeviceHandler() = delete;

    DeviceHandler(Engine* engine)
        : engine_(engine)
    {}

    virtual ~DeviceHandler() {};

    virtual void* GetNativeWindowHandle() const override
    {
        return engine_->GetWindowHandler();
    }
    virtual void SetDrawCommandCalled(bool hasDrawCommand) const override
    {
    }
    virtual void SetDrawCallCount(std::uint32_t drawcall) const override
    {
    }
    virtual void SetBufferCount(std::uint32_t count) const override
    {
    }
    virtual void SetTextureCount(std::uint32_t count) const override
    {
    }
    virtual void SetBufferMemorySize(std::uint32_t size) const override
    {
    }
    virtual void SetPresentDone() const override
    {
    }
    virtual void ShowDebugMessage(const std::string& message) const override
    {
    }
    virtual void ShowDebugMessage(const char* message) const override
    {
    }

protected:
    Engine* engine_ = nullptr;
};

Engine* Engine::engine_ = nullptr;

Engine::Engine(void* windowHandler)
{
    windowHandler_ = windowHandler;
}

bool Engine::Init()
{
    gpuDevice_ = RHI::VKDevice::Create({}, std::make_unique<DeviceHandler>(this));
    LOGI("Engine::Init gpuDevice_=%p", gpuDevice_);
    world_ = new World();
    LOGI("Engine::Init world_=%p", world_);
    renderScene_ = new RenderScene();
    eventSystem_ = new EventSystem(windowHandler_);
    return true;
}

Engine* Engine::CreateEngine(void* windowHandler)
{
    RX_ASSERT(nullptr == engine_);
    Engine::engine_ = new Engine(windowHandler);
    engine_->Init();
    return GetEngine();
}

Engine* Engine::GetEngine()
{
    RX_ASSERT(engine_ != nullptr);
    RX_ASSERT(engine_->gpuDevice_ != nullptr);
    return engine_;
}

Engine::~Engine()
{
    // TODO release
    RX_SAFE_RELEASE(world_);
}

void Engine::Update(float dt)
{
    gpuDevice_->OnFrameCallback(dt);

    if (!world_)return;

    world_->Tick(dt);

    renderScene_->Tick(dt);

    renderScene_->SubmitGPU();
}

void Engine::RunWithWorld(World* world)
{
    RX_SAFE_RETAIN(world);
    RX_SAFE_RELEASE(world_);
    world_ = world;
}

std::uint32_t Engine::StartScheduler(FuncTypeEngineUpdateCallback callback, void* data)
{
    auto id = ++schedulerIDRecorder_;
    schedulers_[id] = { callback, data };
    return id;
}

void Engine::CancelScheduler(std::uint32_t schedulerId)
{
    if (auto it = schedulers_.find(schedulerId); it != schedulers_.end())
    {
        schedulers_.erase(it);
    }
}

NS_RX_END