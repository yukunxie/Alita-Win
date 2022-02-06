//
// Created by realxie on 2019-10-11.
//

#pragma once

#include "Macros.h"

#include <memory>
#include <stdint.h>
#include <algorithm>
#include <thread>
#include <vector>

NS_GFX_BEGIN

class GfxBase;
class Device;
class BindGroup;
class BindGroupLayout;
class BindingResource;
class Buffer;
class BufferBinding;
class CanvasContext;
class CommandBuffer;
class CommandEncoder;
class ComputePipeline;
class ComputePassEncoder;
class Framebuffer;
class Fence;
class PipelineLayout;
class Queue;
class RenderPass;
class RenderPassEncoder;
class RenderPipeline;
class RenderQueue;
class RenderTarget;
class Sampler;
class Shader;
class SwapChain;
class Texture;
class TextureView;
class SampledTextureView;
class DescriptorSet;
class QuerySet;
class SamplerBinding;
class TextureViewBinding;

typedef std::shared_ptr<GfxBase> GfxBasePtr;
typedef std::shared_ptr<Device> DevicePtr;
typedef std::shared_ptr<BindGroup> BindGroupPtr;
typedef std::shared_ptr<BindGroupLayout> BindGroupLayoutPtr;
typedef std::shared_ptr<BindingResource> BindingResourcePtr;
typedef std::shared_ptr<Buffer> BufferPtr;
typedef std::shared_ptr<BufferBinding> BufferBindingPtr;
typedef std::shared_ptr<CanvasContext> CanvasContextPtr;
typedef std::shared_ptr<CommandBuffer> CommandBufferPtr;
typedef std::shared_ptr<CommandEncoder> CommandEncoderPtr;
typedef std::shared_ptr<ComputePipeline> ComputePipelinePtr;
typedef std::shared_ptr<ComputePassEncoder> ComputePassEncoderPtr;
typedef std::shared_ptr<Framebuffer> FramebufferPtr;
typedef std::shared_ptr<Fence> FencePtr;
typedef std::shared_ptr<PipelineLayout> PipelineLayoutPtr;
typedef std::shared_ptr<Queue> QueuePtr;
typedef std::shared_ptr<RenderPass> RenderPassPtr;
typedef std::shared_ptr<RenderPassEncoder> RenderPassEncoderPtr;
typedef std::shared_ptr<RenderPipeline> RenderPipelinePtr;
typedef std::shared_ptr<RenderQueue> RenderQueuePtr;
typedef std::shared_ptr<RenderTarget> RenderTargetPtr;
typedef std::shared_ptr<Sampler> SamplerPtr;
typedef std::shared_ptr<Shader> ShaderPtr;
typedef std::shared_ptr<SwapChain> SwapChainPtr;
typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<TextureView> TextureViewPtr;
typedef std::shared_ptr<SampledTextureView> SampledTextureViewPtr;
typedef std::shared_ptr<DescriptorSet> DescriptorSetPtr;
typedef std::shared_ptr<QuerySet> QuerySetPtr;
typedef std::shared_ptr<SamplerBinding> SamplerBindingPtr;
typedef std::shared_ptr<TextureViewBinding> TextureViewBindingPtr;

enum class PipelineType
{
    Graphic,
    Compute
};

enum class RHIObjectType
{
    UNDEFINED,
    Device,
    BindGroup,
    BindGroupLayout,
    BindingResource,
    Buffer,
    BufferBinding,
    CanvasContext,
    CommandBuffer,
    CommandEncoder,
    ComputePipeline,
    ComputePassEncoder,
    Framebuffer,
    Fence,
    PipelineLayout,
    Queue,
    RenderPass,
    RenderPassEncoder,
    RenderPipeline,
    RenderQueue,
    RenderTarget,
    Sampler,
    Shader,
    SwapChain,
    Texture,
    TextureView,
    SampledTextureView,
    DescriptorSet,
    QuerySet,
    
    MAX_COUNT
};

constexpr std::uint32_t kMaxRHIObjectTypeCount = (std::uint32_t) RHIObjectType::MAX_COUNT;

const char* const RHIObjectTypeNames[kMaxRHIObjectTypeCount] = {
    "UNDEFINED",
    "Device",
    "BindGroup",
    "BindGroupLayout",
    "BindingResource",
    "Buffer",
    "BufferBinding",
    "CanvasContext",
    "CommandBuffer",
    "CommandEncoder",
    "ComputePipeline",
    "ComputePassEncoder",
    "Framebuffer",
    "Fence",
    "PipelineLayout",
    "Queue",
    "RenderPass",
    "RenderPassEncoder",
    "RenderPipeline",
    "RenderQueue",
    "RenderTarget",
    "Sampler",
    "Shader",
    "SwapChain",
    "Texture",
    "TextureView",
    "SampledTextureView",
    "DescriptorSet",
    "QuerySet",
};

class GfxNoncopyable
{
public:
    GfxNoncopyable() = default;
    
    GfxNoncopyable(GfxNoncopyable &&) = default;
    
    GfxNoncopyable &operator=(GfxNoncopyable &&) = default;

private:
    GfxNoncopyable(const GfxNoncopyable &) = delete;
    
    GfxNoncopyable &operator=(const GfxNoncopyable &) = delete;
};

class GfxObjectManager;

class Device;

constexpr std::uint32_t INVALID_OBJECT_ID = std::numeric_limits<std::uint32_t>::max();

class GfxBase : public GfxNoncopyable
{
protected:
    GfxBase(DevicePtr device, RHIObjectType objectType);
    
    GfxBase(RHIObjectType objectType);
    
public:

    virtual ~GfxBase();

    
    FORCE_INLINE RHIObjectType GetObjectType() const
    {
        GFX_ASSERT(objectType__ != RHIObjectType::UNDEFINED);
        return objectType__;
    }
    
    std::uint32_t GetId() const
    {
        return globalId__;
    }
    
    void SetId(std::uint32_t id)
    {
        GFX_ASSERT(globalId__ == INVALID_OBJECT_ID || id == INVALID_OBJECT_ID);
        globalId__ = id;
    }
    
    DevicePtr GetGPUDevice() const
    {
        return GPUDevice_.lock();
    }
    
    void SetScriptObject(void* object)
    { bindingScriptObject_ = object; }
    
    void* GetScriptObject()
    { return bindingScriptObject_; }
    
    virtual void Reset() {}
    
    virtual void Destroy() {};
    
    virtual void OnDeleteEvent() const
    {
        // To override this function to process some logic before delete
    }
    
    virtual void Dispose() = 0;
    
    FORCE_INLINE bool IsDisposed() const
    { return disposed__; }
    
    FORCE_INLINE static void CheckThread()
    {
        // TODO realxie Recreate swapchain时会触发Assert，暂时先关掉
#if defined(GFX_DEBUG) && GFX_DEBUG && 0
        if (mainThreadId_ && mainThreadId_ != pthread_self())
        {
            GFX_ASSERT(false, "Only support in the main thread.");
        }
#endif
    }
    
    FORCE_INLINE static void SetMainThreadId(std::thread::native_handle_type mainThreadId)
    {
#if defined(GFX_DEBUG) && GFX_DEBUG
        mainThreadId_ = mainThreadId;
#endif
    }

protected:
    
    FORCE_INLINE void SetDisposed()
    {
        CheckThread();
        disposed__ = true;
    }

private:
    RHIObjectType objectType__ = RHIObjectType::UNDEFINED;
    bool disposed__ = false;
    std::uint32_t globalId__ = INVALID_OBJECT_ID;
    void* bindingScriptObject_ = nullptr;
    std::weak_ptr<Device> GPUDevice_;
    
#if defined(GFX_DEBUG) && GFX_DEBUG
    static std::thread::native_handle_type mainThreadId_;
#endif
    
    // RHI_TRACE_OBJ_REF_COUNT中的代码用来在资源泄漏时进行问题查找，
#if RHI_TRACE_OBJ_REF_COUNT
public:
    static void PushTraceback(const char* file, const char* funcName, std::uint32_t lineno);
    static void PopTraceback();

protected:
    mutable std::vector<std::string> xxxfuncCallRetainTracebacks_;
    mutable std::vector<std::string> xxxfuncCallReleaseTracebacks_;
#endif

#if AUTO_UNWIND_TRACEBACK_ENABLED
    std::string _unwindTraceback;
#endif
};

// 为了减少Retain/Release带来的不匹配问题，后续考虑所有引用的地方都使用该结构进行自动引用计数
template<typename TP_>
struct RHIObjectWrapper final
{
    RHIObjectWrapper()
    {}
    
    RHIObjectWrapper(TP_* data)
    {
        GFX_PTR_ASSIGN(data_, data);
    }

    RHIObjectWrapper(const RHIObjectWrapper &entity)
    {
        GFX_PTR_ASSIGN(data_, entity.data_);
    }
    
    RHIObjectWrapper(RHIObjectWrapper &&entity)
    {
        GFX_SAFE_RELEASE(data_);
        data_ = entity.data_;
        entity.data_ = nullptr;
    }

    ~RHIObjectWrapper()
    {
        GFX_SAFE_RELEASE(data_);
    }
    
    TP_* Get() const
    {
        return data_;
    }
    
    RHIObjectWrapper<TP_>& operator =(const TP_& __r)
    {
        GFX_PTR_ASSIGN(data_, __r.data_);
        return *this;
    }
    
    RHIObjectWrapper<TP_>& operator =(TP_&& __r)
    {
        GFX_SAFE_RELEASE(data_);
        data_ = __r.data_;
        __r.data_ = nullptr;
        return *this;
    }
    
    RHIObjectWrapper<TP_>& operator =(TP_* __r)
    {
        GFX_PTR_ASSIGN(data_, __r);
        return *this;
    }
    
    RHIObjectWrapper<TP_>& operator =(const TP_* __r)
    {
        GFX_PTR_ASSIGN(data_, __r);
        return *this;
    }

    TP_& operator * () const
    {
        return *data_;
    }
    
    TP_* operator->() const
    {
        return data_;
    }
    
    operator bool() const
    {
        // right to left
        return !!data_;
    }
    
    void Reset()
    {
        GFX_SAFE_RELEASE(data_);
    }

protected:
    TP_* data_ = nullptr;
};



NS_GFX_END
