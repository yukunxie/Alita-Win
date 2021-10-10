//
// Created by realxie on 2019-10-02.
//

#ifndef RHI_DEVICE_H
#define RHI_DEVICE_H

#include "RHIObjectBase.h"
#include "RHIObjectManager.h"
#include "Descriptors.h"

#include <vector>
#include <unordered_map>
#include <map>

NS_RHI_BEGIN

class BindGroup;

class BindGroupLayout;

class BindingResource;

class Buffer;

class CommandBuffer;

class CommandEncoder;

class RenderPipeline;

class PipelineLayout;

class Queue;

class Fence;

class RenderBundleEncoder;

class RenderPass;

class RenderPassEncoder;

class RenderQueue;

class Sampler;

class Shader;

class Texture;

class TextureView;

class SwapChain;

class TextureViewBinding;

class SamplerBinding;

class TextureViewBinding;

class ComputePipeline;

class BufferBinding;

class QuerySet;

class IDeviceExternalDeps
{
public:
    virtual ~IDeviceExternalDeps() {};
    virtual void* GetNativeWindowHandle() const = 0;
    virtual void SetDrawCommandCalled(bool hasDrawCommand) const = 0;
    virtual void SetDrawCallCount(std::uint32_t drawcall) const = 0;
    virtual void SetBufferCount(std::uint32_t count) const = 0;
    virtual void SetTextureCount(std::uint32_t count) const = 0;
    virtual void SetBufferMemorySize(std::uint32_t size) const = 0;
    virtual void SetPresentDone() const = 0;
    virtual void ShowDebugMessage(const std::string& message) const = 0;
    virtual void ShowDebugMessage(const char* message) const = 0;
    
};

typedef std::shared_ptr<Texture> TexturePTR;

class Device : public RHIObjectBase
{
public:
    Device(std::unique_ptr<IDeviceExternalDeps>&& deviceExternalDeps)
        : RHIObjectBase(RHIObjectType::Device)
        , deviceExternalDeps_(std::move(deviceExternalDeps))
    {
        rhiObjectManager_.AddObject(this);
    }
    
    FORCE_INLINE RHIObjectManager &GetObjectManager()
    {
        return rhiObjectManager_;
    }
    
    void AddAutoReleaseObjectToPool(RHIObjectBase* object);
    
    void PurgeAutoReleasePool();
    
    const std::unique_ptr<IDeviceExternalDeps>& GetDeviceExternalDeps() const
    {
        return deviceExternalDeps_;
    }

protected:
    virtual ~Device() = default;

public:
    virtual SwapChain* CreateSwapchain(const SwapChainDescriptor &descriptor) = 0;
    
    virtual TextureFormat GetSwapchainPreferredFormat() = 0;
    
    virtual Buffer* CreateBuffer(const BufferDescriptor &descriptor) = 0;
    
    virtual BufferBinding*
    CreateBufferBinding(Buffer* buffer, BufferSize offset, BufferSize size) = 0;
    
    virtual void WriteBuffer(const Buffer* buffer, const void* data, std::uint32_t offset,
                             std::uint32_t size) = 0;
    
    virtual RenderBundleEncoder* CreateRenderBundleEncoder(
        const RenderBundleEncoderDescriptor &descriptor) = 0;
    
    virtual RenderPipeline* CreateRenderPipeline(RenderPipelineDescriptor &descriptor) = 0;
    
    virtual ComputePipeline* CreateComputePipeline(ComputePipelineDescriptor &descriptor) = 0;
    
    virtual Shader* CreateShaderModule(const ShaderModuleDescriptor &descriptor) = 0;
    
    virtual Texture* CreateTexture(const TextureDescriptor &descriptor) = 0;
    
    virtual Sampler* CreateSampler(const SamplerDescriptor &descriptor = {}) = 0;
    
    virtual SamplerBinding* CreateSamplerBinding(Sampler* sampler) = 0;
    
    virtual TextureViewBinding* CreateTextureViewBinding(TextureView* view) = 0;
    
    virtual BindGroupLayout* CreateBindGroupLayout(const BindGroupLayoutDescriptor &descriptor) = 0;
    
    virtual BindGroup* CreateBindGroup(BindGroupDescriptor &descriptor) = 0;
    
    virtual PipelineLayout* CreatePipelineLayout(const PipelineLayoutDescriptor &descriptor) = 0;
    
    virtual Fence* CreateFence(const FenceDescriptor &descriptor) = 0;
    
    virtual CommandEncoder*
    CreateCommandEncoder(const CommandEncoderDescriptor &descriptor = {}) = 0;
    
    virtual QuerySet* CreateQuerySet(const QuerySetDescriptor& descriptor) = 0;
    
    virtual Queue* GetQueue() = 0;
    
    virtual void OnScriptObjectDestroy()
    {}
    
    virtual void OnEnterBackgroud() = 0;
    
    virtual void OnEnterForeground() = 0;
    
    virtual void OnFrameCallback(float dt) = 0;
    
    virtual void OnFrameEnd() = 0;

    virtual void OnWindowResized() = 0;

private:
    RHIObjectManager rhiObjectManager_;
#if RHI_DEBUG
    std::multimap<RHIObjectType, RHIObjectBase*> autoReleasedObjectPool_;
#else
    std::vector<RHIObjectBase*> autoReleasedObjectPool_;
#endif

protected:
    std::unique_ptr<IDeviceExternalDeps> deviceExternalDeps_;
    std::unordered_map<std::uint32_t, RHIObjectBase*> uniqueIdToObjectMap_;
};

NS_RHI_END

#endif //RHI_DEVICE_H
