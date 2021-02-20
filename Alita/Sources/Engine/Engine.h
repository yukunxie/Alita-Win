//
// Created by realxie on 2019-11-03.
//

#ifndef ALITA_ENGINE_H
#define ALITA_ENGINE_H

#include "RHI.h"
#include "Base/Macros.h"

#include "Graphics/RenderScene.h"

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

class Engine
{
protected:
    Engine(void* data);

    bool Init();

public:
    virtual ~Engine();
    
    static Engine* CreateEngine(void* data);
    
    static Engine* GetEngine();

public:

    RHI::Device* GetGPUDevice()
    {
        return gpuDevice_;
    }

    RenderScene* GetRenderScene()
    {
        return renderScene_;
    }

    RHI::SwapChain* GetSwapchain()
    {
        return rhiSwapChain_;
    }

public:
    void Update(float dt);
    
    void RunWithWorld(World* world);
    
    std::uint32_t StartScheduler(FuncTypeEngineUpdateCallback callback, void* data);
    
    void CancelScheduler(std::uint32_t schedulerId);

protected:
    static Engine* engine_;
    World* world_ = nullptr;
    RenderScene* renderScene_ = nullptr;
    
    std::uint32_t schedulerIDRecorder_ = 0;
    std::map<std::uint32_t, SchedulerEntity> schedulers_;
    
    RHI::Device* gpuDevice_ = nullptr;

    // for testing.
    RHI::CanvasContext* rhiCanvasContext_ = nullptr;
    RHI::Device* rhiDevice_ = nullptr;
    RHI::Buffer* rhiVertexBuffer_ = nullptr;
    RHI::Buffer* rhiIndexBuffer_ = nullptr;
    RHI::Buffer* rhiUniformBuffer_ = nullptr;
    RHI::BindGroup* rhiBindGroup_ = nullptr;
    RHI::BindingResource* rhiBindingBuffer_ = nullptr;
    RHI::BindingResource* rhiBindingCombined_ = nullptr;
    RHI::PipelineLayout* rhiPipelineLayout_ = nullptr;
    RHI::BindGroupLayout* rhiBindGroupLayout_ = nullptr;
    RHI::Texture* rhiTexture_ = nullptr;
    RHI::TextureView* rhiTextureView_ = nullptr;
    RHI::Texture* rhiDSTexture_ = nullptr;
    RHI::TextureView* rhiDSTextureView_ = nullptr;
    RHI::Sampler* rhiSampler_ = nullptr;
    RHI::RenderPipeline* rhiGraphicPipeline_ = nullptr;
    RHI::Shader* rhiVertShader_ = nullptr;
    RHI::Shader* rhiFragShader_ = nullptr;
    RHI::CommandEncoder* rhiCommandEncoder_ = nullptr;
    RHI::SwapChain* rhiSwapChain_ = nullptr;
};

NS_RX_END


#endif //ALITA_ENGINE_H
