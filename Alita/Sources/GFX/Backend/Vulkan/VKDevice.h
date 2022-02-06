//
// Created by realxie on 2019-10-02.
//

#ifndef RHI_VKDEVICE_H
#define RHI_VKDEVICE_H

#include "GFX/GFX.h"
#include "GFX/Helper/Vector.h"
#include "GFX/xxhash64.h"

#include "Platform/Platform.h"


#include "VulkanMacros.h"
#include "VKTypes.h"
#include "VKDevice_Helper.h"

#include "VMA/VmaUsage.h"

#if ANDROID
#include "vulkan_wrapper.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_android.h>
#include <android/native_window.h>
#endif

#include <chrono>
#include <vector>
#include <map>
#include <unordered_map>
#include <bitset>
#include <array>
#include <functional>
#include <atomic>

#define USE_VULKAN_MEMORY_ALLCATOR 0

NS_GFX_BEGIN

#define VKDEVICE() GFX_CAST(VKDevice*, GetGPUDevice())

#if defined(GFX_DEBUG) && GFX_DEBUG
#define VULKAN_VALIDATE_LAYER_ENABLED 1
#else
#define VULKAN_VALIDATE_LAYER_ENABLED 1
#endif

// Only enable validate layer in debug model
#if defined(NDEBUG) && NDEBUG && VULKAN_VALIDATE_LAYER_ENABLED
#undef VULKAN_VALIDATE_LAYER_ENABLED
#define VULKAN_VALIDATE_LAYER_ENABLED 0
#endif

class VKBuffer;
class VKRenderPipeline;
class VKComputePipeline;
class VKShader;
class VKQueue;
class VKSwapChain;
class VKFramebuffer;
class VKCommandBuffer;
class VKRenderPassEncoder;
class VKComputePassEncoder;
class VKRenderPass;
class VKTexture;
class VKTextureView;
class TextureViewManager;
class BindGroupLayout;
class AsyncWorkerVulkan;
class VKQuerySet;


void DumpVulkanErrorMessage(VkResult code, const char* filename, uint32_t lineno);

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct DefaultSwapchainConfig
{
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkColorSpaceKHR colorSpaceKhr;
    Extent2D extent2D;
    std::uint32_t imageCount = 3;
    VkSurfaceCapabilitiesKHR capabilities;
};

struct QueueFamilyIndices
{
    int graphicsFamily = -1;
    int presentFamily = -1;
    int computeFamily = -1;
    
    bool isComplete()
    {
        return graphicsFamily >= 0 && presentFamily >= 0 && computeFamily >= 0;
    }
};

class VKDevice final : public Device
{
public:
    static DevicePtr Create(const DeviceDescriptor &descriptor, std::unique_ptr<IDeviceExternalDeps>&& deviceExternalDeps);

protected:
    VKDevice(std::unique_ptr<IDeviceExternalDeps>&& deviceExternalDeps);
    
    bool Init(const DeviceDescriptor &descriptor);

public:
    ~VKDevice();
    
    FORCE_INLINE VkDevice GetNative() const
    { return vkDevice_; }
    
    void SetPresentDone(bool hasDrawCall)
    { deviceExternalDeps_->SetPresentDone(); }
    
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    
    VkDescriptorPool GetDescriptorPool()
    { return vkDescriptorPool_; }
    
    std::uint32_t GetGraphicQueueFamilyIndex() const
    { return graphicQueueFamilyIndex_; }
    
    VkCommandPool GetCommandBufferPool() const
    { return vkCommandPool_; }
    
    const QueueFamilyIndices &GetQueueFamilyIndices() const
    { return queueFamilyIndices_; }
    
    void AddWaitingSemaphore(VkSemaphore semaphore)
    { waitingSemaphores_.push_back(semaphore); }
    
    const std::vector<VkSemaphore> &GetWaitingSemaphores()
    { return waitingSemaphores_; }
    
    void ClearWaitingSemaphores()
    { waitingSemaphores_.clear(); }
    
    RenderPassPtr GetOrCreateRenderPass(const RenderPassCacheQuery &query);
    
    FramebufferPtr GetOrCreateFramebuffer(const FramebufferCacheQuery &query);
    
    bool HasExtension(const std::vector<VkExtensionProperties> &extensions, const char* extension);
    
    bool SupportDebugGroup()
    { return supportDebugGroup_; }
    
    void SetSwapChain(const SwapChainPtr& swapChain)
    { this->swapChain_ = swapChain; }
    
    const SwapChainPtr& GetSwapChain()
    { return this->swapChain_; }
    
    const DefaultSwapchainConfig &GetSwapchainConfig()
    { return defaultSwapchainConfig_; }
    
    void InvalidateFramebuffers();
    
    bool CheckGameThread()
    {
        return true;
        //return pthread_self() == gameThreadId_;
    }
    
    bool IsPendingDestroyed()
    { return pendingDestroyed_; }

public:
    virtual SwapChainPtr CreateSwapchain(const SwapChainDescriptor &descriptor) override;
    
    virtual TextureFormat GetSwapchainPreferredFormat() override;
    
    virtual BufferPtr CreateBuffer(const BufferDescriptor &descriptor) override;
    
    virtual BufferBindingPtr
    CreateBufferBinding(BufferPtr buffer, BufferSize offset, BufferSize size) override;
    
    virtual void WriteBuffer(const Buffer* buffer, const void* data, std::uint32_t offset,
                             std::uint32_t size) override;
    
    virtual RenderPipelinePtr
    CreateRenderPipeline(RenderPipelineDescriptor &descriptor) override;
    
    virtual ComputePipelinePtr CreateComputePipeline(ComputePipelineDescriptor &descriptor) override;
    
    virtual ShaderPtr CreateShaderModule(const ShaderModuleDescriptor &descriptor) override;
    
    virtual TexturePtr CreateTexture(const TextureDescriptor &descriptor) override;

    virtual TextureViewPtr CreateTextureView(const TexturePtr& vkTexture, const TextureViewDescriptor& descriptor) override;
    
    virtual SamplerPtr CreateSampler(const SamplerDescriptor &descriptor = {}) override;
    
    virtual SamplerBindingPtr CreateSamplerBinding(SamplerPtr sampler) override;
    
    virtual TextureViewBindingPtr CreateTextureViewBinding(TextureViewPtr view) override;
    
    virtual BindGroupLayoutPtr
    CreateBindGroupLayout(const BindGroupLayoutDescriptor &descriptor) override;
    
    virtual BindGroupPtr CreateBindGroup(BindGroupDescriptor &descriptor) override;
    
    virtual PipelineLayoutPtr
    CreatePipelineLayout(const PipelineLayoutDescriptor &descriptor) override;
    
    virtual CommandEncoderPtr
    CreateCommandEncoder(const CommandEncoderDescriptor &descriptor = {}) override;
    
    virtual FencePtr CreateFence(const FenceDescriptor &descriptor) override;
    
    virtual QuerySetPtr CreateQuerySet(const QuerySetDescriptor &descriptor) override;
    
    virtual QueuePtr GetQueue() override;
    
    virtual void OnScriptObjectDestroy() override;
    
    virtual void OnEnterBackgroud() override;
    
    virtual void OnEnterForeground() override;
    
    virtual void OnFrameCallback(float dt) override;
    
    virtual void OnFrameEnd() override;

    virtual void OnWindowResized() override;
    
    void CheckOnBackgroud()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [=] { return !isOnBackground_; });
    }
    
    bool IsSupportNegativeViewport()
    {
#ifdef ANDROID
        return isSupportNegativeViewport_;
#else
        return false;
#endif
    }

public:
    template<typename PtrType_, typename _VKType, typename ...Args>
    FORCE_INLINE PtrType_ CreateObject(Args ...args)
    {
        _VKType* obj = new _VKType(GetDevicePtr());
        if (obj && !obj->Init(args...))
        {
            GFX_SAFE_RELEASE(obj);
        }

        return PtrType_(obj);
    }

    const DevicePtr& GetDevicePtr();
    
    CommandBufferPtr CreateCommandBuffer();
    
    TextureViewManager* GetTextureViewManager()
    {
        return pTextureViewMgr_.get();
    }
    
    const Viewport &GetDefaultViewport()
    {
        return defaultViewport_;
    }
    
    AsyncWorker* GetAsyncWorker()
    { return pAsyncWorker_.get(); }
    
    VkSemaphore AcquireVkSemaphore();
    
    void ReturnVkSemaphore(VkSemaphore vkSemaphore);
    
    VkFence AcquireVkFence();
    
    void ReturnVkFence(VkFence vkFence);
    
    void ScheduleCallbackExecutedInGameThread(const std::function<void(DevicePtr)> &callback);
    
    template<typename AsyncTaskName, typename ...Args>
    void ScheduleAsyncTask(Args ...args)
    {
        auto task = std::make_shared<AsyncTaskName>(args...);
#if USE_RENDER_THREAD || 1
        GetAsyncWorker()->EnqueueInGameThread(task);
#else
        task->Execute();
#endif
    }
    
    void SetAsyncTaskDoneInRenderingThread(AsyncTaskPtr task)
    {
        SCOPED_LOCK(mutexPendingDoneTask_);
        pendingDoneTasks_.push_back(task);
    }

#if USE_VULKAN_MEMORY_ALLCATOR
    
    VmaAllocator GetVmaAllocator()
    { return vmaAllocator_; }

#endif

private:
    void AttemptEnumerateInstanceLayerAndExtensions();
    
    void AttemptEnumerateDeviceLayerAndExtensions();
    
    bool InitInstance();
    
    bool CreateSurface(VkFormat surfaceFormat = VK_FORMAT_R8G8B8A8_UNORM);
    
    bool InitPhysicDevice();
    
    bool InitDevice();
    
    bool InitCommandPool();
    
    bool InitDescriptorPool();
    
    QueuePtr CreateQueue();
    
    void ReleaseCaches();

private:
    bool InitQueueFamilies();
    
    VkSurfaceFormatKHR
    ChooseVulkanSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats,
                              VkFormat targetSurfaceFormat);
    
    VkPresentModeKHR
    ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
    
    Extent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    
    SwapChainSupportDetails QuerySwapChainSupport(VkSurfaceKHR surface);
    
    bool IsDeviceSuitable(VkPhysicalDevice device);
    
    VkSurfaceKHR GetVulkanSurface() const
    { return vkSurface_; }
    
    virtual void Dispose() override;
    
    void QueryDeviceFeatures();
    
    void QueryDeviceMemoryProperties();

private:
    std::thread::native_handle_type gameThreadId_ = 0;
    
    std::unique_ptr<AsyncWorker> pAsyncWorker_ = nullptr;
    
    VkInstance vkInstance_ = VK_NULL_HANDLE;
    VkDevice vkDevice_ = VK_NULL_HANDLE;
    VkPhysicalDevice vkPhysicalDevice_ = VK_NULL_HANDLE;
    VkCommandPool vkCommandPool_ = VK_NULL_HANDLE;
    VkDescriptorPool vkDescriptorPool_ = VK_NULL_HANDLE;
    VkSurfaceKHR vkSurface_ = VK_NULL_HANDLE;

#if USE_VULKAN_MEMORY_ALLCATOR
    VmaAllocator vmaAllocator_ = nullptr;
#endif
    
    VkPhysicalDeviceFeatures deviceFeatures_;
    VkPhysicalDeviceMemoryProperties memoryProperties_;

#if VULKAN_VALIDATE_LAYER_ENABLED
    VkDebugReportCallbackEXT vkDebugReportCallback_ = 0L;
    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT_ = nullptr;
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT_ = nullptr;
#endif
    
    QueueFamilyIndices queueFamilyIndices_;
    std::uint32_t graphicQueueFamilyIndex_ = 0;
    
    DefaultSwapchainConfig defaultSwapchainConfig_;
    
    std::unique_ptr<TextureViewManager> pTextureViewMgr_;
    
    Viewport defaultViewport_;
    SwapChainPtr swapChain_ = nullptr;
    QueuePtr renderQueue_ = nullptr;
    QueuePtr imageLayoutTransistQueue_ = nullptr;
    
    std::vector<VkSemaphore> waitingSemaphores_;
    RenderPassCache renderPassCache_;
    FramebufferCache framebufferCache_;
    
    std::vector<std::pair<RenderPipelineDescriptor, RenderPipelinePtr>> renderPipelineCache_;
    std::vector<std::pair<BindGroupLayoutDescriptor, BindGroupLayoutPtr>> bindGroupLayoutCache_;
    std::vector<std::pair<PipelineLayoutDescriptor, PipelineLayoutPtr>> pipelineLayoutCache_;
    std::vector<VkFence> vkFenceCache_;
    std::vector<AsyncTaskPtr> pendingDoneTasks_;
    std::condition_variable cv_;
    
    std::vector<std::function<void(DevicePtr)>> scheduledAsyncCallbacks_;
    
    std::mutex mutex_;
    std::mutex mutexPendingDoneTask_;
    std::mutex mutexScheduleAsyncCallback_;
    
    bool supportDebugGroup_ = false;
    bool isOnBackground_ = false;
    bool pendingDestroyed_ = false;
    bool isSupportNegativeViewport_ = false;
    
    std::vector<const char*> instanceLayerNames_;
    std::vector<const char*> deviceLayerNames_;
    std::vector<VkExtensionProperties> instanceExtensions_;
    std::vector<VkExtensionProperties> deviceExtensions_;
    
    friend class VKSwapChain;

};

NS_GFX_END


#endif //RHI_VKDEVICE_H
