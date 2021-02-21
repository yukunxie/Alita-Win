//
// Created by realxie on 2019-11-03.
//

#include "Engine.h"
#include "World/World.h"
#include "Base/FileSystem.h"

#include "Backend/Vulkan/VKCanvasContext.h"
#include "Backend/Vulkan/ShaderHelper.h"

NS_RX_BEGIN

Engine* Engine::engine_ = nullptr;

Engine::Engine(void* data)
{
    gpuDevice_ = RHI::CreateDeviceHelper(RHI::DeviceType::VULKAN, data);

    {
        rhiCanvasContext_ = new RHI::VKCanvasContext();
        RHI::SwapChainDescriptor swapChainDescriptor;
        swapChainDescriptor.device = gpuDevice_;
        swapChainDescriptor.format = RHI::TextureFormat::BGRA8UNORM;
        rhiSwapChain_ = rhiCanvasContext_->ConfigureSwapChain(swapChainDescriptor);

        //rhiCommandEncoder_ = gpuDevice_->CreateCommandEncoder();
    }
}

bool Engine::Init()
{
    /*std::string shaderText = FileSystem::GetInstance()->GetStringData("Shaders/shader.vert.gl");

    auto spirv = RHI::CompileGLSLToSPIRV(shaderText, RHI::ShaderType::VERTEX);*/

    //gpuDevice_->CreateShaderModule()

    world_ = new World();
    renderScene_ = new RenderScene();
    return true;
}

Engine* Engine::CreateEngine(void* data)
{
    RX_ASSERT(nullptr == engine_);
    Engine::engine_ = new Engine(data);
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
    if (!world_)return;
    
    world_->Tick(dt);

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
    schedulers_[id] = {callback, data};
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