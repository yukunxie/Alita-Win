//
// Created by realxie on 2019-10-02.
//

#ifndef RHI_DEVICE_H
#define RHI_DEVICE_H

#include "GFXBase.h"
#include "GFXObjectManager.h"
#include "Descriptors.h"

#include <vector>
#include <unordered_map>
#include <map>

NS_GFX_BEGIN

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

class Device : public GfxBase
{
public:
    Device(std::unique_ptr<IDeviceExternalDeps>&& deviceExternalDeps)
        : GfxBase(RHIObjectType::Device)
        , deviceExternalDeps_(std::move(deviceExternalDeps))
    {
        rhiObjectManager_.AddObject(this);
    }
    
    FORCE_INLINE GfxObjectManager &GetObjectManager()
    {
        return rhiObjectManager_;
    }
    
    const std::unique_ptr<IDeviceExternalDeps>& GetDeviceExternalDeps() const
    {
        return deviceExternalDeps_;
    }

    virtual ~Device() = default;

public:
    virtual SwapChainPtr CreateSwapchain(const SwapChainDescriptor &descriptor) = 0;
    
    virtual TextureFormat GetSwapchainPreferredFormat() = 0;
    
    virtual BufferPtr CreateBuffer(const BufferDescriptor &descriptor) = 0;
    
    virtual BufferBindingPtr
    CreateBufferBinding(BufferPtr buffer, BufferSize offset, BufferSize size) = 0;
    
    virtual void WriteBuffer(const Buffer* buffer, const void* data, std::uint32_t offset,
                             std::uint32_t size) = 0;
    
    virtual RenderPipelinePtr CreateRenderPipeline(RenderPipelineDescriptor &descriptor) = 0;
    
    virtual ComputePipelinePtr CreateComputePipeline(ComputePipelineDescriptor &descriptor) = 0;
    
    virtual ShaderPtr CreateShaderModule(const ShaderModuleDescriptor &descriptor) = 0;
    
    virtual TexturePtr CreateTexture(const TextureDescriptor &descriptor) = 0;

    virtual TextureViewPtr CreateTextureView(const TexturePtr& vkTexture, const TextureViewDescriptor& descriptor) = 0;
    
    virtual SamplerPtr CreateSampler(const SamplerDescriptor &descriptor = {}) = 0;
    
    virtual SamplerBindingPtr CreateSamplerBinding(SamplerPtr sampler) = 0;
    
    virtual TextureViewBindingPtr CreateTextureViewBinding(TextureViewPtr view) = 0;
    
    virtual BindGroupLayoutPtr CreateBindGroupLayout(const BindGroupLayoutDescriptor &descriptor) = 0;
    
    virtual BindGroupPtr CreateBindGroup(BindGroupDescriptor &descriptor) = 0;
    
    virtual PipelineLayoutPtr CreatePipelineLayout(const PipelineLayoutDescriptor &descriptor) = 0;
    
    virtual FencePtr CreateFence(const FenceDescriptor &descriptor) = 0;
    
    virtual CommandEncoderPtr
    CreateCommandEncoder(const CommandEncoderDescriptor &descriptor = {}) = 0;
    
    virtual QuerySetPtr CreateQuerySet(const QuerySetDescriptor& descriptor) = 0;
    
    virtual QueuePtr GetQueue() = 0;
    
    virtual void OnScriptObjectDestroy()
    {}
    
    virtual void OnEnterBackgroud() = 0;
    
    virtual void OnEnterForeground() = 0;
    
    virtual void OnFrameCallback(float dt) = 0;
    
    virtual void OnFrameEnd() = 0;

    virtual void OnWindowResized() = 0;

private:
    GfxObjectManager rhiObjectManager_;

protected:
    std::unique_ptr<IDeviceExternalDeps> deviceExternalDeps_;
    std::unordered_map<std::uint32_t, GfxBase*> uniqueIdToObjectMap_;
};

NS_GFX_END

#endif //RHI_DEVICE_H
