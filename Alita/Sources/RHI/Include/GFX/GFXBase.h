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
    RenderBundle,
    RenderBundleEncoder,
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
    "RenderBundle",
    "RenderBundleEncoder",
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
    GfxBase(Device* device, RHIObjectType objectType);
    
    GfxBase(RHIObjectType objectType);
    
    virtual ~GfxBase();
    
public:
    
    GfxBase* AutoRelease();
    
    FORCE_INLINE RHIObjectType GetObjectType() const
    {
        RHI_ASSERT(objectType__ != RHIObjectType::UNDEFINED);
        return objectType__;
    }
    
    std::uint32_t GetId() const
    {
        return globalId__;
    }
    
    void SetId(std::uint32_t id)
    {
        RHI_ASSERT(globalId__ == INVALID_OBJECT_ID || id == INVALID_OBJECT_ID);
        globalId__ = id;
    }
    
    Device* GetGPUDevice() const
    {
        return GPUDevice_;
    }
    
    void Retain() const;
    
    void Release() const;
    
    FORCE_INLINE std::int32_t GetReferenceCount() const
    {
        return referenceCount__;
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
#if defined(RHI_DEBUG) && RHI_DEBUG && 0
        if (mainThreadId_ && mainThreadId_ != pthread_self())
        {
            RHI_ASSERT(false, "Only support in the main thread.");
        }
#endif
    }
    
    FORCE_INLINE static void SetMainThreadId(std::thread::native_handle_type mainThreadId)
    {
#if defined(RHI_DEBUG) && RHI_DEBUG
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
    bool autoReleased_ = false;
    // referenceCount__初始化为1，因此需要对其进行AutoRelease
    mutable std::int32_t referenceCount__ = 1;
    std::uint32_t globalId__ = INVALID_OBJECT_ID;
    void* bindingScriptObject_ = nullptr;
    Device* GPUDevice_ = nullptr;
    
#if defined(RHI_DEBUG) && RHI_DEBUG
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
        RHI_PTR_ASSIGN(data_, data);
    }

    RHIObjectWrapper(const RHIObjectWrapper &entity)
    {
        RHI_PTR_ASSIGN(data_, entity.data_);
    }
    
    RHIObjectWrapper(RHIObjectWrapper &&entity)
    {
        RHI_SAFE_RELEASE(data_);
        data_ = entity.data_;
        entity.data_ = nullptr;
    }

    ~RHIObjectWrapper()
    {
        RHI_SAFE_RELEASE(data_);
    }
    
    TP_* Get() const
    {
        return data_;
    }
    
    RHIObjectWrapper<TP_>& operator =(const TP_& __r)
    {
        RHI_PTR_ASSIGN(data_, __r.data_);
        return *this;
    }
    
    RHIObjectWrapper<TP_>& operator =(TP_&& __r)
    {
        RHI_SAFE_RELEASE(data_);
        data_ = __r.data_;
        __r.data_ = nullptr;
        return *this;
    }
    
    RHIObjectWrapper<TP_>& operator =(TP_* __r)
    {
        RHI_PTR_ASSIGN(data_, __r);
        return *this;
    }
    
    RHIObjectWrapper<TP_>& operator =(const TP_* __r)
    {
        RHI_PTR_ASSIGN(data_, __r);
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
        RHI_SAFE_RELEASE(data_);
    }

protected:
    TP_* data_ = nullptr;
};

NS_GFX_END
