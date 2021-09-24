//
// Created by realxie on 2019-10-02.
//

#include "RenderThreading.h"
#include "VKDevice.h"
#include "VulkanMacros.h"
#include "VKBuffer.h"
#include "VKRenderPipeline.h"
#include "VKShader.h"
#include "VKRenderPass.h"
#include "VKTexture.h"
#include "VKTextureView.h"
#include "VKSampler.h"
#include "VKTextureView.h"
#include "VKTextureViewManager.h"
#include "VKPipelineLayout.h"
#include "VKBindGroupLayout.h"
#include "VKBindGroup.h"
#include "VKQueue.h"
#include "VKQuerySet.h"
#include "VKRenderBundleEncoder.h"
#include "VKRenderQueue.h"
#include "VKRenderPass.h"
#include "VKRenderPassEncoder.h"
#include "VKCommandEncoder.h"
#include "VKCommandBuffer.h"
#include "VKComputePassEncoder.h"
#include "VKSwapChain.h"
#include "VKFramebuffer.h"
#include "VKFence.h"
#include "RHI/xxhash64.h"
#include "CommandList.h"

#include <vector>
#include <array>
#include <string.h>

#if ANDROID
#include <android/log.h>
#endif

NS_RHI_BEGIN

PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT = nullptr;
PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT = nullptr;
PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT = nullptr;
PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT = nullptr;
PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT = nullptr;

PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
VkDebugUtilsMessengerEXT debugUtilsMessenger;

XXHash64 gXXHash64Generator(0x21378732);

bool gIsDeviceSupportNegativeViewport = false;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
    VkDebugReportFlagsEXT msgFlags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t srcObject, size_t location,
    int32_t msgCode, const char* pLayerPrefix,
    const char* pMsg, void* pUserData);

void DumpVulkanErrorMessage(VkResult code, const char* filename, uint32_t lineno);

// 目前仅处理单个WebGPU实例的情况，该变量用来标记是否有Device未释放
static VKDevice* sActivedDevice_ = nullptr;

VKDevice* VKDevice::Create(const DeviceDescriptor &descriptor, std::unique_ptr<IDeviceExternalDeps>&& deviceExternalDeps)
{
#if defined(RHI_DEBUG) && RHI_DEBUG
    RHIObjectBase::SetMainThreadId(pthread_self());
#endif
    
    auto device = new VKDevice(std::move(deviceExternalDeps));
    if (device && device->Init(descriptor))
    {
        device->AutoRelease();
        return device;
    }
    
    if (device)
    {
        device->Release();
    }
    return nullptr;
}

VKDevice::VKDevice(std::unique_ptr<IDeviceExternalDeps>&& deviceExternalDeps)
: Device(std::move(deviceExternalDeps))
{
    if (sActivedDevice_)
    {
        deviceExternalDeps_->ShowDebugMessage("Fatal Error, VKDevice has been leaked!");
        RHI_ASSERT(sActivedDevice_ == nullptr);
        LOGW("Force delete sActivedDevice_");
        RHI_SAFE_DELETE(sActivedDevice_);
    }
    sActivedDevice_ = this;
    
    //LOGI("device thread-id: %p", (void*)pthread_self());
    
    pTextureViewMgr_ = std::make_unique<VKTextureViewManager>(this);
   /* 
    gameThreadId_ = pthread_self();
    
    int policy;
    struct sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);
    LOGI("GLThread's Priority %d: policy=%d, min=%d, max=%d", param.sched_priority, policy, sched_get_priority_min(policy), sched_get_priority_max(policy));*/
}

bool VKDevice::Init(const DeviceDescriptor &descriptor)
{
    do {
#if ANDROID
        if (!vkLoadVulkanABIs()) break;
#endif
    
        LOGI("InitInstance");
        if (!InitInstance()) break;
        
        LOGI("InitPhysicDevice");
        if (!InitPhysicDevice()) break;
    
        LOGI("CreateSurface");
        if (!CreateSurface()) break;
    
        LOGI("InitDevice");
        if (!InitDevice()) break;
    
        LOGI("InitQueueFamilies");
        if (!InitQueueFamilies()) break;
    
        LOGI("InitCommandPool");
        if (!InitCommandPool()) break;
    
        LOGI("InitDescriptorPool");
        if (!InitDescriptorPool()) break;
    
        LOGI("Create AsyncWorkerVulkan");
        pAsyncWorker_ = std::make_unique<AsyncWorkerVulkan>(this);
        LOGI("VKDevice AsyncWorkerVulkan pAsyncWorker_=%p.", pAsyncWorker_.get());
        
        return true;
        
    } while (0);
    
    LOGE("VKDevice::Init fail.");
    return false;
}

void VKDevice::ReleaseCaches()
{
    LOGW("VKDevice::ReleaseCaches begin.");
    
    for (auto tp : renderPipelineCache_)
    {
        RHI_SAFE_RELEASE(tp.second);
    }
    renderPipelineCache_.clear();
    
    for (auto tp : bindGroupLayoutCache_)
    {
        RHI_SAFE_RELEASE(tp.second);
    }
    bindGroupLayoutCache_.clear();
    
    for (auto tp : pipelineLayoutCache_)
    {
        RHI_SAFE_RELEASE(tp.second);
    }
    pipelineLayoutCache_.clear();
    
    if (!freeCommandLists_.empty())
    {
        SCOPED_LOCK(mutex_);
        freeCommandLists_.clear();
    }
    
    if (!scheduledAsyncCallbacks_.empty())
    {
        SCOPED_LOCK(mutexScheduleAsyncCallback_);
        
        for (auto &callback : scheduledAsyncCallbacks_)
        {
            callback(this);
        }
        scheduledAsyncCallbacks_.clear();
    }
    
    if (!pendingReleaseCommandLists_.empty())
    {
        SCOPED_LOCK(mutex_);
        pendingReleaseCommandLists_.clear();
    }
    
    if (!scheduledAsyncCallbacks_.empty())
    {
        SCOPED_LOCK(mutex_);
        
        for (auto &callback : scheduledAsyncCallbacks_)
        {
            callback(this);
        }
        scheduledAsyncCallbacks_.clear();
    }
    
    if (!pendingDoneTasks_.empty())
    {
        SCOPED_LOCK(mutexPendingDoneTask_);
        pendingDoneTasks_.clear();
    }
    
    for (auto &it : renderPassCache_)
    {
        it.second->Release();
    }
    renderPassCache_.clear();
    
    for (auto &it : framebufferCache_)
    {
        it.second->Release();
    }
    framebufferCache_.clear();
    
    RHI_SAFE_RELEASE(renderQueue_);
    
    RHI_SAFE_RELEASE(imageLayoutTransistQueue_);
    
    if (pTextureViewMgr_)
    {
        pTextureViewMgr_->Purge();
    }
    
    LOGW("VKDevice::ReleaseCaches done.");
}

void VKDevice::OnScriptObjectDestroy()
{
    LOGW("VKDevice::OnScriptObjectDestroy begin.");
    
    pendingDestroyed_ = true;
    PurgeAutoReleasePool();
    if (pAsyncWorker_)
    {
        pAsyncWorker_->Stop();
    }
    pAsyncWorker_.reset();
    vkDeviceWaitIdle(vkDevice_);
    ReleaseCaches();
    
    LOGW("VKDevice::OnScriptObjectDestroy done.");
}

void VKDevice::OnEnterBackgroud()
{
    isOnBackground_ = true;
    LOGI(" VKDevice::OnEnterBackgroud");
}

void VKDevice::OnEnterForeground()
{
    isOnBackground_ = false;
    cv_.notify_all();
    LOGI(" VKDevice::OnEnterForeground");
}

void VKDevice::OnFrameCallback(float dt)
{
    // This logic must be executed firstly.
    if (!scheduledAsyncCallbacks_.empty())
    {
        SCOPED_LOCK(mutexScheduleAsyncCallback_);
        
        for (auto &callback : scheduledAsyncCallbacks_)
        {
            callback(this);
        }
        scheduledAsyncCallbacks_.clear();
    }
    
    if (!pendingDoneTasks_.empty())
    {
        SCOPED_LOCK(mutexPendingDoneTask_);
        pendingDoneTasks_.clear();
    }
    
    if (!pendingReleaseCommandLists_.empty())
    {
        SCOPED_LOCK(mutex_);
        
        for (auto commandQueue: pendingReleaseCommandLists_)
        {
            commandQueue->Reset();
            freeCommandLists_.push_back(commandQueue);
        }
        pendingReleaseCommandLists_.clear();
    }
    
    PurgeAutoReleasePool();
}

void VKDevice::OnFrameEnd()
{
    RHI_CAST(VKQueue*, renderQueue_)->SubmitInternal();
}

void VKDevice::Dispose()
{
    RHI_DISPOSE_BEGIN();
    
    ReleaseCaches();
    
    for (auto vkFence: vkFenceCache_)
    {
        vkDestroyFence(vkDevice_, vkFence, nullptr);
    }
    vkFenceCache_.clear();
    
    if (vkSurface_)
    {
        LOGI("Destroy Surface");
        vkDestroySurfaceKHR(vkInstance_, vkSurface_, nullptr);
        vkSurface_ = VK_NULL_HANDLE;
    }
    
    if (vkDescriptorPool_)
    {
        LOGI("Destroy DescriptorPool");
        vkDestroyDescriptorPool(vkDevice_, vkDescriptorPool_, nullptr);
        vkDescriptorPool_ = VK_NULL_HANDLE;
    }
    
    if (vkCommandPool_)
    {
        LOGI("Destroy CommandPool");
        vkDestroyCommandPool(vkDevice_, vkCommandPool_, nullptr);
        vkCommandPool_ = VK_NULL_HANDLE;
    }

#if USE_VULKAN_MEMORY_ALLCATOR
    LOGI("Destroy vmaAllocator");
    vmaDestroyAllocator(vmaAllocator_);
    LOGI("Destroy vmaAllocator done.");
#endif
    
    if (vkDevice_)
    {
        LOGI("Destroy Device");
        vkDeviceWaitIdle(vkDevice_);
        vkDestroyDevice(vkDevice_, nullptr);
        vkDevice_ = VK_NULL_HANDLE;
    }

#if VULKAN_VALIDATE_LAYER_ENABLED
    if (vkDestroyDebugReportCallbackEXT_ && vkDebugReportCallback_)
    {
        vkDestroyDebugReportCallbackEXT_(vkInstance_, vkDebugReportCallback_, nullptr);
        vkDebugReportCallback_ = VK_NULL_HANDLE;
    }
#endif
    
    if (vkInstance_)
    {
        LOGI("Destroy Instance");
        vkDestroyInstance(vkInstance_, nullptr);
        vkInstance_ = VK_NULL_HANDLE;
    }
    
    vkPhysicalDevice_ = VK_NULL_HANDLE;
    
    pTextureViewMgr_.reset();
    
    LOGI("-------------VKDevice has been disposed successfully.--------------");
}

VKDevice::~VKDevice()
{
    Dispose();
    sActivedDevice_ = nullptr;
}

VKRenderPass* VKDevice::GetOrCreateRenderPass(const RenderPassCacheQuery &query)
{
    auto it = renderPassCache_.find(query);
    if (it != renderPassCache_.end())
    {
        return it->second;
    }
    
    auto renderPass = CreateObject<VKRenderPass>(query);
    renderPassCache_[query] = renderPass;
    RHI_SAFE_RETAIN(renderPass);
    
    return renderPass;
}

VKFramebuffer* VKDevice::GetOrCreateFramebuffer(const FramebufferCacheQuery &query)
{
    SCOPED_LOCK(mutex_);
    
    auto it = framebufferCache_.find(query);
    if (it != framebufferCache_.end())
    {
        return it->second;
    }
    
    auto framebuffer = CreateObject<VKFramebuffer>(query);
    framebufferCache_[query] = framebuffer;
    RHI_SAFE_RETAIN(framebuffer);
    
    return framebuffer;
}

SwapChain* VKDevice::CreateSwapchain(const SwapChainDescriptor &descriptor)
{
    if (swapChain_)
    {
        return swapChain_;
    }
    return CreateObject<VKSwapChain>(descriptor);
}

TextureFormat VKDevice::GetSwapchainPreferredFormat()
{
    return TextureFormat::RGBA8UNORM;
}

Buffer* VKDevice::CreateBuffer(const BufferDescriptor &descriptor)
{
    auto buffer = CreateObject<VKBuffer>(descriptor);
    if (buffer)
    {
        GetObjectManager().increaseBufferSize(descriptor.size);
    }
    return buffer;
}

BufferBinding* VKDevice::CreateBufferBinding(Buffer* buffer, BufferSize offset, BufferSize size)
{
    return CreateObject<BufferBinding>(buffer, offset, size);
}

void VKDevice::WriteBuffer(const Buffer* buffer, const void* data, std::uint32_t offset,
                           std::uint32_t size)
{
    ((VKBuffer*) buffer)->UpdateBuffer(data, offset, size);
}

static VkApplicationInfo sVulkanAPPInfo = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = nullptr,
    .pApplicationName = "Alita",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = "RX",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_MAKE_VERSION(1, 2, 0),
};

void VKDevice::AttemptEnumerateInstanceLayerAndExtensions()
{
    std::vector<VkLayerProperties> instanceLayers;

//#if VULKAN_VALIDATE_LAYER_ENABLED
#if _DEBUG

    uint32_t layerPropCount = 0;
    vkEnumerateInstanceLayerProperties(&layerPropCount, nullptr);
    
    // Enumerate layers with valid pointer in last parameter
    instanceLayers.resize(layerPropCount);
    vkEnumerateInstanceLayerProperties(&layerPropCount, instanceLayers.data());
    
    for (const auto &layer : instanceLayers)
    {
        LOGI("vulkan-Find instance layer: %s", layer.layerName);
    }
    
    const char* requestVulkanLayerNames[] = {
        //"VK_LAYER_GOOGLE_threading",
        //"VK_LAYER_LUNARG_parameter_validation",
        //"VK_LAYER_LUNARG_object_tracker",
        //"VK_LAYER_LUNARG_core_validation",
        //"VK_LAYER_GOOGLE_unique_objects",
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_RENDERDOC_Capture",
    };
    
    for (uint32_t j = 0; j < sizeof(requestVulkanLayerNames) / sizeof(requestVulkanLayerNames[0]); j++)
    {
        const char* layerName = requestVulkanLayerNames[j];
        bool found = false;
        for (const auto &layer: instanceLayers)
        {
            found = strcmp(layer.layerName, layerName) == 0;
            if (found) break;
        }
        if (!found) continue;
    
        instanceLayerNames_.push_back(layerName);
    }
#endif
    
    // 如果加载的snapdragon profiler 或 RenderDoc的提供的layer, 则禁用所有的layer，否则会crash
    bool hasLoadProfilingLayer = false;
    for (const auto &layer: instanceLayers)
    {
        hasLoadProfilingLayer = hasLoadProfilingLayer || strcmp(layer.layerName, "sdp") == 0;
        hasLoadProfilingLayer = hasLoadProfilingLayer || strcmp(layer.layerName, "VK_LAYER_RENDERDOC_Capture") == 0;
    }
    
    if (hasLoadProfilingLayer)
    {
        instanceLayerNames_.clear();
        LOGI("vulkan has load SDP or RenderDoc layer, all instance layers disabled.");
    }
    
    for (const auto &layer : instanceLayerNames_)
    {
        LOGI("vulkan-Find loaded instance layer: %s", layer);
    }
    
    auto enumerateExtensionsFromLayer = [this](const char* layerName) {
        // Get the instance extension count
        uint32_t extCount = 0;
        vkEnumerateInstanceExtensionProperties(layerName, &extCount, nullptr);
        
        // Enumerate the instance extensions
        std::vector<VkExtensionProperties> extensions(extCount);
        vkEnumerateInstanceExtensionProperties(layerName, &extCount, extensions.data());
        
        for (VkExtensionProperties &ext : extensions)
        {
            bool found = false;
            for (size_t j = 0; (j < instanceExtensions_.size()) && (!found); ++j)
            {
                found = strcmp(ext.extensionName, instanceExtensions_[j].extensionName) == 0;
            }
            if (!found)
            {
                instanceExtensions_.push_back(ext);
            }
        }
    };
    
    // Get the default extensions from in
    enumerateExtensionsFromLayer(nullptr);
    
    // Get the extensions from concrete layer.
    for (size_t i = 0; i < instanceLayerNames_.size(); ++i)
    {
        enumerateExtensionsFromLayer(instanceLayerNames_[i]);
    }
    
    for (const auto &ext : instanceExtensions_)
    {
        LOGI("vulkan-Find instance extension: %s : 0x%X", ext.extensionName, ext.specVersion);
    }
}

void VKDevice::AttemptEnumerateDeviceLayerAndExtensions()
{
    uint32_t layerPropCount = 0;
    vkEnumerateDeviceLayerProperties(vkPhysicalDevice_, &layerPropCount, nullptr);
    
    // Enumerate layers with valid pointer in last parameter
    std::vector<VkLayerProperties> layers(layerPropCount);
    vkEnumerateDeviceLayerProperties(vkPhysicalDevice_, &layerPropCount, layers.data());
    
    const char* requestVulkanLayerNames[] = {
        "sdp", // For snapdragon profiler
#if VULKAN_VALIDATE_LAYER_ENABLED || WIN32
        "VK_LAYER_GOOGLE_threading",
        "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_core_validation",
        "VK_LAYER_GOOGLE_unique_objects",
        "VK_LAYER_LUNARG_parameter_validation",
#endif
    };
    
    for (uint32_t j = 0; j < sizeof(requestVulkanLayerNames) / sizeof(requestVulkanLayerNames[0]); j++)
    {
        const char* layerName = requestVulkanLayerNames[j];
        bool found = false;
        for (const auto &layer: layers)
        {
            found = strcmp(layer.layerName, layerName) == 0;
            if (found) break;
        }
        if (!found) continue;
        
        deviceLayerNames_.push_back(layerName);
    }
    
    for (const auto &layer : deviceLayerNames_)
    {
        LOGI("vulkan-Find device layer: %s", layer);
    }
    
    auto enumerateExtensionsFromLayer = [this](const char* layerName) {
        // Get the device extension count
        uint32_t extCount = 0;
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice_, layerName, &extCount, nullptr);
        
        // Enumerate the device extensions
        std::vector<VkExtensionProperties> extensions(extCount);
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice_, layerName, &extCount,
                                               extensions.data());
        
        for (VkExtensionProperties &ext : extensions)
        {
            bool found = false;
            for (size_t j = 0; (j < deviceExtensions_.size()) && (!found); ++j)
            {
                found = strcmp(ext.extensionName, deviceExtensions_[j].extensionName) == 0;
            }
            if (!found)
            {
                deviceExtensions_.push_back(ext);
            }
        }
    };
    
    // Get the default extensions from in
    enumerateExtensionsFromLayer(nullptr);
    
    // Get the extensions from concrete layer.
    for (size_t i = 0; i < deviceLayerNames_.size(); ++i)
    {
        enumerateExtensionsFromLayer(deviceLayerNames_[i]);
    }
    
    for (const auto &ext : deviceExtensions_)
    {
        LOGI("vulkan-Find device extension: %s : 0x%X", ext.extensionName, ext.specVersion);
    }
}

bool VKDevice::InitInstance()
{
    AttemptEnumerateInstanceLayerAndExtensions();
    
    // prepare necessary extensions: Vulkan on Android need these to function
    std::vector<const char*> instanceExt;
    instanceExt.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#ifdef ANDROID
    instanceExt.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

    // Enable Vulkan debug callback.
    if (HasExtension(instanceExtensions_, VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
    {
        instanceExt.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    if (HasExtension(instanceExtensions_, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        instanceExt.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    
    // Create the Vulkan instance
    VkInstanceCreateInfo instanceCreateInfo;
    {
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.pApplicationInfo = &sVulkanAPPInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExt.size());
        instanceCreateInfo.ppEnabledExtensionNames = instanceExt.data();
        instanceCreateInfo.enabledLayerCount = (std::uint32_t)instanceLayerNames_.size();
        instanceCreateInfo.ppEnabledLayerNames = instanceLayerNames_.data();
    };

#ifdef WIN32
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*>  strExtensions;
    {
        for (int i = 0; i < glfwExtensionCount; ++i)
        {
            strExtensions.push_back(glfwExtensions[i]);
        }
    }
    if (HasExtension(instanceExtensions_, VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
    {
        strExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    if (HasExtension(instanceExtensions_, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        strExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    instanceCreateInfo.enabledExtensionCount = strExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = strExtensions.data();
#endif
    
    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance_);
    if (VK_SUCCESS != result)
    {
        DumpVulkanErrorMessage(result, __FILE__, __LINE__);
        const char* tips = R"(
            refer: https://developer.android.com/ndk/guides/graphics/validation-layer#enable-layers-outside-app
            // Check the global settings
            $ adb shell settings list global | grep gpu
                enable_gpu_debug_layers=1
                gpu_debug_app=com.realxie.demo
                gpu_debug_layers=VK_LAYER_PROFILER_Capture
            // Delete these settings.
            $ adb shell settings delete global enable_gpu_debug_layers
            $ adb shell settings delete global gpu_debug_app
            $ adb shell settings delete global gpu_debug_layers
            $ adb shell settings delete global gpu_debug_layer_app
        )";
        const char* fmt = R"(To use Vulkan, maybe the global settings have been modified -
                by debug tools(Snapdragon Profiler or RenderDoc), \nreset the global settings, -
                as follow instructions and ~Reboot~ the device.: %s)";
        LOGW(fmt, tips);
    }
    
    if (VK_NULL_HANDLE == vkInstance_)
    {
        return false;
    }
    
    vkDebugMarkerSetObjectTagEXT = PFN_vkDebugMarkerSetObjectTagEXT(vkGetInstanceProcAddr(vkInstance_, "vkDebugMarkerSetObjectTagEXT"));
    vkDebugMarkerSetObjectNameEXT = PFN_vkDebugMarkerSetObjectNameEXT(vkGetInstanceProcAddr(vkInstance_, "vkDebugMarkerSetObjectNameEXT"));
    vkCmdDebugMarkerBeginEXT = PFN_vkCmdDebugMarkerBeginEXT(vkGetInstanceProcAddr(vkInstance_, "vkCmdDebugMarkerBeginEXT"));
    vkCmdDebugMarkerEndEXT = PFN_vkCmdDebugMarkerEndEXT(vkGetInstanceProcAddr(vkInstance_, "vkCmdDebugMarkerEndEXT"));
    vkCmdDebugMarkerInsertEXT = PFN_vkCmdDebugMarkerInsertEXT(vkGetInstanceProcAddr(vkInstance_, "vkCmdDebugMarkerInsertEXT"));
    
    LOGI("vkDebugMarkerSetObjectTagEXT -  %p", vkDebugMarkerSetObjectTagEXT);
    LOGI("vkDebugMarkerSetObjectNameEXT -  %p", vkDebugMarkerSetObjectNameEXT);
    LOGI("vkCmdDebugMarkerBeginEXT -  %p", vkCmdDebugMarkerBeginEXT);
    LOGI("vkCmdDebugMarkerEndEXT -  %p", vkCmdDebugMarkerEndEXT);
    LOGI("vkCmdDebugMarkerInsertEXT -  %p", vkCmdDebugMarkerInsertEXT);

#if VULKAN_VALIDATE_LAYER_ENABLED
    vkCreateDebugReportCallbackEXT_ = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(
        vkInstance_, "vkCreateDebugReportCallbackEXT");
    vkDestroyDebugReportCallbackEXT_ = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(
        vkInstance_, "vkDestroyDebugReportCallbackEXT");

    vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vkInstance_, "vkCreateDebugUtilsMessengerEXT"));
    
    // Create the debug callback with desired settings
    if (vkCreateDebugReportCallbackEXT_)
    {
        VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo;
        debugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debugReportCallbackCreateInfo.pNext = NULL;
        debugReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                              VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                              VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debugReportCallbackCreateInfo.pfnCallback = DebugReportCallback;
        debugReportCallbackCreateInfo.pUserData = NULL;
        
        CALL_VK(vkCreateDebugReportCallbackEXT_(vkInstance_, &debugReportCallbackCreateInfo,
                                                nullptr, &vkDebugReportCallback_));
    }
#endif
    
    return true;
}

bool VKDevice::CreateSurface(VkFormat surfaceFormat /*= VK_FORMAT_R8G8B8A8_UNORM*/)
{
    RHI_ASSERT(vkInstance_ != nullptr);

#ifdef WIN32
    GLFWwindow* window = (GLFWwindow*)deviceExternalDeps_->GetNativeWindowHandle();
    CALL_VK(glfwCreateWindowSurface(vkInstance_, window, NULL, &vkSurface_));
#elif defined(ANDROID)
    ANativeWindow* window = (ANativeWindow*) deviceExternalDeps_->GetNativeWindowHandle();
    if (!window)
    {
        LOGE("VKDevice CreateSurface fail, window is null.");
        return false;
    }
    
    // if we create a surface, we need the surface extension
    VkAndroidSurfaceCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .window = window,
    };
    
    if (vkSurface_)
    {
        vkDestroySurfaceKHR(vkInstance_, vkSurface_, nullptr);
        vkSurface_ = VK_NULL_HANDLE;
    }
    CALL_VK(vkCreateAndroidSurfaceKHR(vkInstance_, &createInfo, nullptr, &vkSurface_));
#endif
    
    
    
    // Set default viewport
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice_, vkSurface_,
                                                &surfaceCapabilities);
    
    LOGI("Vulkan Surface Capabilities:\n");
    LOGI("\timage count: %u - %u\n", surfaceCapabilities.minImageCount,
         surfaceCapabilities.maxImageCount);
    LOGI("\tarray layers: %u\n", surfaceCapabilities.maxImageArrayLayers);
    LOGI("\timage size (now): %dx%d\n", surfaceCapabilities.currentExtent.width,
         surfaceCapabilities.currentExtent.height);
    LOGI("\timage size (extent): %dx%d - %dx%d\n", surfaceCapabilities.minImageExtent.width,
         surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.width,
         surfaceCapabilities.maxImageExtent.height);
    LOGI("\tusage: %x\n", surfaceCapabilities.supportedUsageFlags);
    LOGI("\tcurrent transform: %u\n", surfaceCapabilities.currentTransform);
    LOGI("\tallowed transforms: %x\n", surfaceCapabilities.supportedTransforms);
    LOGI("\tcomposite alpha flags: %u\n", surfaceCapabilities.currentTransform);
    
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(vkSurface_);
    VkSurfaceFormatKHR surfaceFormatKhr = ChooseVulkanSurfaceFormat(swapChainSupport.formats,
                                                                    surfaceFormat);
    
    uint32_t minImageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        minImageCount > swapChainSupport.capabilities.maxImageCount)
    {
        minImageCount = swapChainSupport.capabilities.maxImageCount;
    }
    
    defaultSwapchainConfig_.format = ToVulkanType(GetSwapchainPreferredFormat());// surfaceFormatKhr.format;
    defaultSwapchainConfig_.colorSpaceKhr = surfaceFormatKhr.colorSpace;
    defaultSwapchainConfig_.extent2D = ChooseSwapExtent(swapChainSupport.capabilities);
    defaultSwapchainConfig_.imageCount = minImageCount;
    defaultSwapchainConfig_.capabilities = swapChainSupport.capabilities;
    
    // defaultSwapchainConfig_.extent2D.width = 720;
    // defaultSwapchainConfig_.extent2D.height = 1334;
    
    defaultViewport_.width = defaultSwapchainConfig_.extent2D.width;
    defaultViewport_.height = defaultSwapchainConfig_.extent2D.height;
    defaultViewport_.minDepth = 0.0f;
    defaultViewport_.maxDepth = 1.0f;
    defaultViewport_.x = 0;
    defaultViewport_.y = 0;
    
    return true;
}

bool VKDevice::InitPhysicDevice()
{
    RHI_ASSERT(vkInstance_ != nullptr);
    
    uint32_t gpuCount = 0;
    CALL_VK(vkEnumeratePhysicalDevices(vkInstance_, &gpuCount, nullptr));
    if (gpuCount == 0)
    {
        LOGE("CreatePhysicDevice fail, no physical devices.");
        return false;
    }
    std::vector<VkPhysicalDevice> tmpGpus(gpuCount, VK_NULL_HANDLE);
    CALL_VK(vkEnumeratePhysicalDevices(vkInstance_, &gpuCount, tmpGpus.data()));
    
    // Pick up the first GPU Device
    vkPhysicalDevice_ = tmpGpus[0];
    
    QueryDeviceFeatures();
    
    QueryDeviceMemoryProperties();
    
    return true;
}

void VKDevice::QueryDeviceFeatures()
{
    RHI_ASSERT(vkPhysicalDevice_);
    
    vkGetPhysicalDeviceFeatures(vkPhysicalDevice_, &deviceFeatures_);
    
    LOGI("GPU Feature: robustBufferAccess=%s",
         deviceFeatures_.robustBufferAccess ? "True" : "False");
    LOGI("GPU Feature: fullDrawIndexUint32=%s",
         deviceFeatures_.fullDrawIndexUint32 ? "True" : "False");
    LOGI("GPU Feature: imageCubeArray=%s", deviceFeatures_.imageCubeArray ? "True" : "False");
    LOGI("GPU Feature: independentBlend=%s", deviceFeatures_.independentBlend ? "True" : "False");
    LOGI("GPU Feature: geometryShader=%s", deviceFeatures_.geometryShader ? "True" : "False");
    LOGI("GPU Feature: tessellationShader=%s",
         deviceFeatures_.tessellationShader ? "True" : "False");
    LOGI("GPU Feature: sampleRateShading=%s", deviceFeatures_.sampleRateShading ? "True" : "False");
    LOGI("GPU Feature: dualSrcBlend=%s", deviceFeatures_.dualSrcBlend ? "True" : "False");
    LOGI("GPU Feature: logicOp=%s", deviceFeatures_.logicOp ? "True" : "False");
    LOGI("GPU Feature: multiDrawIndirect=%s", deviceFeatures_.multiDrawIndirect ? "True" : "False");
    LOGI("GPU Feature: drawIndirectFirstInstance=%s",
         deviceFeatures_.drawIndirectFirstInstance ? "True" : "False");
    LOGI("GPU Feature: depthClamp=%s", deviceFeatures_.depthClamp ? "True" : "False");
    LOGI("GPU Feature: depthBiasClamp=%s", deviceFeatures_.depthBiasClamp ? "True" : "False");
    LOGI("GPU Feature: fillModeNonSolid=%s", deviceFeatures_.fillModeNonSolid ? "True" : "False");
    LOGI("GPU Feature: depthBounds=%s", deviceFeatures_.depthBounds ? "True" : "False");
    LOGI("GPU Feature: wideLines=%s", deviceFeatures_.wideLines ? "True" : "False");
    LOGI("GPU Feature: largePoints=%s", deviceFeatures_.largePoints ? "True" : "False");
    LOGI("GPU Feature: alphaToOne=%s", deviceFeatures_.alphaToOne ? "True" : "False");
    LOGI("GPU Feature: multiViewport=%s", deviceFeatures_.multiViewport ? "True" : "False");
    LOGI("GPU Feature: samplerAnisotropy=%s", deviceFeatures_.samplerAnisotropy ? "True" : "False");
    LOGI("GPU Feature: textureCompressionETC2=%s",
         deviceFeatures_.textureCompressionETC2 ? "True" : "False");
    LOGI("GPU Feature: textureCompressionASTC_LDR=%s",
         deviceFeatures_.textureCompressionASTC_LDR ? "True" : "False");
    LOGI("GPU Feature: textureCompressionBC=%s",
         deviceFeatures_.textureCompressionBC ? "True" : "False");
    LOGI("GPU Feature: occlusionQueryPrecise=%s",
         deviceFeatures_.occlusionQueryPrecise ? "True" : "False");
    LOGI("GPU Feature: pipelineStatisticsQuery=%s",
         deviceFeatures_.pipelineStatisticsQuery ? "True" : "False");
    LOGI("GPU Feature: vertexPipelineStoresAndAtomics=%s",
         deviceFeatures_.vertexPipelineStoresAndAtomics ? "True" : "False");
    LOGI("GPU Feature: fragmentStoresAndAtomics=%s",
         deviceFeatures_.fragmentStoresAndAtomics ? "True" : "False");
    LOGI("GPU Feature: shaderTessellationAndGeometryPointSize=%s",
         deviceFeatures_.shaderTessellationAndGeometryPointSize ? "True" : "False");
    LOGI("GPU Feature: shaderImageGatherExtended=%s",
         deviceFeatures_.shaderImageGatherExtended ? "True" : "False");
    LOGI("GPU Feature: shaderStorageImageExtendedFormats=%s",
         deviceFeatures_.shaderStorageImageExtendedFormats ? "True" : "False");
    LOGI("GPU Feature: shaderStorageImageMultisample=%s",
         deviceFeatures_.shaderStorageImageMultisample ? "True" : "False");
    LOGI("GPU Feature: shaderStorageImageReadWithoutFormat=%s",
         deviceFeatures_.shaderStorageImageReadWithoutFormat ? "True" : "False");
    LOGI("GPU Feature: shaderStorageImageWriteWithoutFormat=%s",
         deviceFeatures_.shaderStorageImageWriteWithoutFormat ? "True" : "False");
    LOGI("GPU Feature: shaderUniformBufferArrayDynamicIndexing=%s",
         deviceFeatures_.shaderUniformBufferArrayDynamicIndexing ? "True" : "False");
    LOGI("GPU Feature: shaderSampledImageArrayDynamicIndexing=%s",
         deviceFeatures_.shaderSampledImageArrayDynamicIndexing ? "True" : "False");
    LOGI("GPU Feature: shaderStorageBufferArrayDynamicIndexing=%s",
         deviceFeatures_.shaderStorageBufferArrayDynamicIndexing ? "True" : "False");
    LOGI("GPU Feature: shaderStorageImageArrayDynamicIndexing=%s",
         deviceFeatures_.shaderStorageImageArrayDynamicIndexing ? "True" : "False");
    LOGI("GPU Feature: shaderClipDistance=%s",
         deviceFeatures_.shaderClipDistance ? "True" : "False");
    LOGI("GPU Feature: shaderCullDistance=%s",
         deviceFeatures_.shaderCullDistance ? "True" : "False");
    LOGI("GPU Feature: shaderFloat64=%s", deviceFeatures_.shaderFloat64 ? "True" : "False");
    LOGI("GPU Feature: shaderInt64=%s", deviceFeatures_.shaderInt64 ? "True" : "False");
    LOGI("GPU Feature: shaderInt16=%s", deviceFeatures_.shaderInt16 ? "True" : "False");
    LOGI("GPU Feature: shaderResourceResidency=%s",
         deviceFeatures_.shaderResourceResidency ? "True" : "False");
    LOGI("GPU Feature: shaderResourceMinLod=%s",
         deviceFeatures_.shaderResourceMinLod ? "True" : "False");
    LOGI("GPU Feature: sparseBinding=%s", deviceFeatures_.sparseBinding ? "True" : "False");
    LOGI("GPU Feature: sparseResidencyBuffer=%s",
         deviceFeatures_.sparseResidencyBuffer ? "True" : "False");
    LOGI("GPU Feature: sparseResidencyImage2D=%s",
         deviceFeatures_.sparseResidencyImage2D ? "True" : "False");
    LOGI("GPU Feature: sparseResidencyImage3D=%s",
         deviceFeatures_.sparseResidencyImage3D ? "True" : "False");
    LOGI("GPU Feature: sparseResidency2Samples=%s",
         deviceFeatures_.sparseResidency2Samples ? "True" : "False");
    LOGI("GPU Feature: sparseResidency4Samples=%s",
         deviceFeatures_.sparseResidency4Samples ? "True" : "False");
    LOGI("GPU Feature: sparseResidency8Samples=%s",
         deviceFeatures_.sparseResidency8Samples ? "True" : "False");
    LOGI("GPU Feature: sparseResidency16Samples=%s",
         deviceFeatures_.sparseResidency16Samples ? "True" : "False");
    LOGI("GPU Feature: sparseResidencyAliased=%s",
         deviceFeatures_.sparseResidencyAliased ? "True" : "False");
    LOGI("GPU Feature: variableMultisampleRate=%s",
         deviceFeatures_.variableMultisampleRate ? "True" : "False");
    LOGI("GPU Feature: inheritedQueries=%s", deviceFeatures_.inheritedQueries ? "True" : "False");
    
    LOGI("------------------------------------------------------------------------");
    // Limits
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice_, &deviceProperties);
    VkPhysicalDeviceLimits &limits = deviceProperties.limits;
    LOGI("GPU Limit: maxImageDimension1D=%u", limits.maxImageDimension1D);
    LOGI("GPU Limit: maxImageDimension2D=%u", limits.maxImageDimension2D);
    LOGI("GPU Limit: maxImageDimension3D=%u", limits.maxImageDimension3D);
    LOGI("GPU Limit: maxImageDimensionCube=%u", limits.maxImageDimensionCube);
    LOGI("GPU Limit: maxImageArrayLayers=%u", limits.maxImageArrayLayers);
    LOGI("GPU Limit: maxTexelBufferElements=%u", limits.maxTexelBufferElements);
    LOGI("GPU Limit: maxUniformBufferRange=%u", limits.maxUniformBufferRange);
    LOGI("GPU Limit: maxStorageBufferRange=%u", limits.maxStorageBufferRange);
    LOGI("GPU Limit: maxPushConstantsSize=%u", limits.maxPushConstantsSize);
    LOGI("GPU Limit: maxMemoryAllocationCount=%u", limits.maxMemoryAllocationCount);
    LOGI("GPU Limit: maxSamplerAllocationCount=%u", limits.maxSamplerAllocationCount);
    LOGI("GPU Limit: bufferImageGranularity=%u", (std::uint32_t) limits.bufferImageGranularity);
    LOGI("GPU Limit: sparseAddressSpaceSize=%u", (std::uint32_t) limits.sparseAddressSpaceSize);
    LOGI("GPU Limit: maxBoundDescriptorSets=%u", limits.maxBoundDescriptorSets);
    LOGI("GPU Limit: maxPerStageDescriptorSamplers=%u", limits.maxPerStageDescriptorSamplers);
    LOGI("GPU Limit: maxPerStageDescriptorUniformBuffers=%u",
         limits.maxPerStageDescriptorUniformBuffers);
    LOGI("GPU Limit: maxPerStageDescriptorStorageBuffers=%u",
         limits.maxPerStageDescriptorStorageBuffers);
    LOGI("GPU Limit: maxPerStageDescriptorSampledImages=%u",
         limits.maxPerStageDescriptorSampledImages);
    LOGI("GPU Limit: maxPerStageDescriptorStorageImages=%u",
         limits.maxPerStageDescriptorStorageImages);
    LOGI("GPU Limit: maxPerStageDescriptorInputAttachments=%u",
         limits.maxPerStageDescriptorInputAttachments);
    LOGI("GPU Limit: maxPerStageResources=%u", limits.maxPerStageResources);
    LOGI("GPU Limit: maxDescriptorSetSamplers=%u", limits.maxDescriptorSetSamplers);
    LOGI("GPU Limit: maxDescriptorSetUniformBuffers=%u", limits.maxDescriptorSetUniformBuffers);
    LOGI("GPU Limit: maxDescriptorSetUniformBuffersDynamic=%u",
         limits.maxDescriptorSetUniformBuffersDynamic);
    LOGI("GPU Limit: maxDescriptorSetStorageBuffers=%u", limits.maxDescriptorSetStorageBuffers);
    LOGI("GPU Limit: maxDescriptorSetStorageBuffersDynamic=%u",
         limits.maxDescriptorSetStorageBuffersDynamic);
    LOGI("GPU Limit: maxDescriptorSetSampledImages=%u", limits.maxDescriptorSetSampledImages);
    LOGI("GPU Limit: maxDescriptorSetStorageImages=%u", limits.maxDescriptorSetStorageImages);
    LOGI("GPU Limit: maxDescriptorSetInputAttachments=%u", limits.maxDescriptorSetInputAttachments);
    LOGI("GPU Limit: maxVertexInputAttributes=%u", limits.maxVertexInputAttributes);
    LOGI("GPU Limit: maxVertexInputBindings=%u", limits.maxVertexInputBindings);
    LOGI("GPU Limit: maxVertexInputAttributeOffset=%u", limits.maxVertexInputAttributeOffset);
    LOGI("GPU Limit: maxVertexInputBindingStride=%u", limits.maxVertexInputBindingStride);
    LOGI("GPU Limit: maxVertexOutputComponents=%u", limits.maxVertexOutputComponents);
    LOGI("GPU Limit: maxTessellationGenerationLevel=%u", limits.maxTessellationGenerationLevel);
    LOGI("GPU Limit: maxTessellationPatchSize=%u", limits.maxTessellationPatchSize);
    LOGI("GPU Limit: maxTessellationControlPerVertexInputComponents=%u",
         limits.maxTessellationControlPerVertexInputComponents);
    LOGI("GPU Limit: maxTessellationControlPerVertexOutputComponents=%u",
         limits.maxTessellationControlPerVertexOutputComponents);
    LOGI("GPU Limit: maxTessellationControlPerPatchOutputComponents=%u",
         limits.maxTessellationControlPerPatchOutputComponents);
    LOGI("GPU Limit: maxTessellationControlTotalOutputComponents=%u",
         limits.maxTessellationControlTotalOutputComponents);
    LOGI("GPU Limit: maxTessellationEvaluationInputComponents=%u",
         limits.maxTessellationEvaluationInputComponents);
    LOGI("GPU Limit: maxTessellationEvaluationOutputComponents=%u",
         limits.maxTessellationEvaluationOutputComponents);
    LOGI("GPU Limit: maxGeometryShaderInvocations=%u", limits.maxGeometryShaderInvocations);
    LOGI("GPU Limit: maxGeometryInputComponents=%u", limits.maxGeometryInputComponents);
    LOGI("GPU Limit: maxGeometryOutputComponents=%u", limits.maxGeometryOutputComponents);
    LOGI("GPU Limit: maxGeometryOutputVertices=%u", limits.maxGeometryOutputVertices);
    LOGI("GPU Limit: maxGeometryTotalOutputComponents=%u", limits.maxGeometryTotalOutputComponents);
    LOGI("GPU Limit: maxFragmentInputComponents=%u", limits.maxFragmentInputComponents);
    LOGI("GPU Limit: maxFragmentOutputAttachments=%u", limits.maxFragmentOutputAttachments);
    LOGI("GPU Limit: maxFragmentDualSrcAttachments=%u", limits.maxFragmentDualSrcAttachments);
    LOGI("GPU Limit: maxFragmentCombinedOutputResources=%u",
         limits.maxFragmentCombinedOutputResources);
    LOGI("GPU Limit: maxComputeSharedMemorySize=%u", limits.maxComputeSharedMemorySize);
    LOGI("GPU Limit: maxComputeWorkGroupCount[3]=%u %u %u", limits.maxComputeWorkGroupCount[0],
         limits.maxComputeWorkGroupCount[1], limits.maxComputeWorkGroupCount[2]);
    LOGI("GPU Limit: maxComputeWorkGroupInvocations=%u", limits.maxComputeWorkGroupInvocations);
    LOGI("GPU Limit: maxComputeWorkGroupSize[3]=%u %u %u", limits.maxComputeWorkGroupSize[0],
         limits.maxComputeWorkGroupSize[1], limits.maxComputeWorkGroupSize[2]);
    LOGI("GPU Limit: subPixelPrecisionBits=%u", limits.subPixelPrecisionBits);
    LOGI("GPU Limit: subTexelPrecisionBits=%u", limits.subTexelPrecisionBits);
    LOGI("GPU Limit: mipmapPrecisionBits=%u", limits.mipmapPrecisionBits);
    LOGI("GPU Limit: maxDrawIndexedIndexValue=%u", limits.maxDrawIndexedIndexValue);
    LOGI("GPU Limit: maxDrawIndirectCount=%u", limits.maxDrawIndirectCount);
    LOGI("GPU Limit: maxSamplerLodBias=%f", limits.maxSamplerLodBias);
    LOGI("GPU Limit: maxSamplerAnisotropy=%f", limits.maxSamplerAnisotropy);
    LOGI("GPU Limit: maxViewports=%u", limits.maxViewports);
    LOGI("GPU Limit: maxViewportDimensions[2]=%u, %u", limits.maxViewportDimensions[0],
         limits.maxViewportDimensions[1]);
    LOGI("GPU Limit: viewportBoundsRange[2]=%f, %f", limits.viewportBoundsRange[0],
         limits.viewportBoundsRange[1]);
    LOGI("GPU Limit: viewportSubPixelBits=%u", limits.viewportSubPixelBits);
    LOGI("GPU Limit: minMemoryMapAlignment=%u", (std::uint32_t) limits.minMemoryMapAlignment);
    LOGI("GPU Limit: minTexelBufferOffsetAlignment=%u",
         (std::uint32_t) limits.minTexelBufferOffsetAlignment);
    LOGI("GPU Limit: minUniformBufferOffsetAlignment=%u",
         (std::uint32_t) limits.minUniformBufferOffsetAlignment);
    LOGI("GPU Limit: minStorageBufferOffsetAlignment=%u",
         (std::uint32_t) limits.minStorageBufferOffsetAlignment);
    LOGI("GPU Limit: minTexelOffset=%d", limits.minTexelOffset);
    LOGI("GPU Limit: maxTexelOffset=%u", limits.maxTexelOffset);
    LOGI("GPU Limit: minTexelGatherOffset=%d", limits.minTexelGatherOffset);
    LOGI("GPU Limit: maxTexelGatherOffset=%u", limits.maxTexelGatherOffset);
    LOGI("GPU Limit: minInterpolationOffset=%f", limits.minInterpolationOffset);
    LOGI("GPU Limit: maxInterpolationOffset=%f", limits.maxInterpolationOffset);
    LOGI("GPU Limit: subPixelInterpolationOffsetBits=%u", limits.subPixelInterpolationOffsetBits);
    LOGI("GPU Limit: maxFramebufferWidth=%u", limits.maxFramebufferWidth);
    LOGI("GPU Limit: maxFramebufferHeight=%u", limits.maxFramebufferHeight);
    LOGI("GPU Limit: maxFramebufferLayers=%u", limits.maxFramebufferLayers);
    LOGI("GPU Limit: framebufferColorSampleCounts=0x%X", limits.framebufferColorSampleCounts);
    LOGI("GPU Limit: framebufferDepthSampleCounts=0x%X", limits.framebufferDepthSampleCounts);
    LOGI("GPU Limit: framebufferStencilSampleCounts=0x%X", limits.framebufferStencilSampleCounts);
    LOGI("GPU Limit: framebufferNoAttachmentsSampleCounts=0x%X",
         limits.framebufferNoAttachmentsSampleCounts);
    LOGI("GPU Limit: maxColorAttachments=%u", limits.maxColorAttachments);
    LOGI("GPU Limit: sampledImageColorSampleCounts=0x%X", limits.sampledImageColorSampleCounts);
    LOGI("GPU Limit: sampledImageIntegerSampleCounts=0x%X",
         limits.sampledImageIntegerSampleCounts);
    LOGI("GPU Limit: sampledImageDepthSampleCounts=0x%X", limits.sampledImageDepthSampleCounts);
    LOGI("GPU Limit: sampledImageStencilSampleCounts=0x%X",
         limits.sampledImageStencilSampleCounts);
    LOGI("GPU Limit: storageImageSampleCounts=0x%X", limits.storageImageSampleCounts);
    LOGI("GPU Limit: maxSampleMaskWords=%u", limits.maxSampleMaskWords);
    LOGI("GPU Limit: timestampComputeAndGraphics=%s",
         limits.timestampComputeAndGraphics == VK_TRUE ? "True" : "False");
    LOGI("GPU Limit: timestampPeriod=%f", limits.timestampPeriod);
    LOGI("GPU Limit: maxClipDistances=%u", limits.maxClipDistances);
    LOGI("GPU Limit: maxCullDistances=%u", limits.maxCullDistances);
    LOGI("GPU Limit: maxCombinedClipAndCullDistances=%u", limits.maxCombinedClipAndCullDistances);
    LOGI("GPU Limit: discreteQueuePriorities=%u", limits.discreteQueuePriorities);
    LOGI("GPU Limit: pointSizeRange[2]=%.f, %f", limits.pointSizeRange[0],
         limits.pointSizeRange[1]);
    LOGI("GPU Limit: lineWidthRange[2]=%f, %f", limits.lineWidthRange[0],
         limits.lineWidthRange[1]);
    LOGI("GPU Limit: pointSizeGranularity=%f", limits.pointSizeGranularity);
    LOGI("GPU Limit: lineWidthGranularity=%f", limits.lineWidthGranularity);
    LOGI("GPU Limit: strictLines=%s", limits.strictLines == VK_TRUE ? "True" : "False");
    LOGI("GPU Limit: standardSampleLocations=%s",
         limits.standardSampleLocations == VK_TRUE ? "True" : "False");
    LOGI("GPU Limit: optimalBufferCopyOffsetAlignment=%u",
         (std::uint32_t) limits.optimalBufferCopyOffsetAlignment);
    LOGI("GPU Limit: optimalBufferCopyRowPitchAlignment=%u",
         (std::uint32_t) limits.optimalBufferCopyRowPitchAlignment);
    LOGI("GPU Limit: nonCoherentAtomSize=%u", (std::uint32_t) limits.nonCoherentAtomSize);
    
}

void VKDevice::QueryDeviceMemoryProperties()
{
    RHI_ASSERT(vkPhysicalDevice_);
    
    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice_, &memoryProperties_);
    
    for (uint32_t i = 0; i < memoryProperties_.memoryTypeCount; i++)
    {
        VkMemoryType memoryType = memoryProperties_.memoryTypes[i];

#define PARSE_MEMORY_PROPERTY(PROPERTY)     \
    if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_##PROPERTY##_BIT)\
        flags +=  flags.empty()? #PROPERTY : std::string(" | ") + #PROPERTY;
        
        std::string flags = "";
        
        PARSE_MEMORY_PROPERTY(DEVICE_LOCAL)
        PARSE_MEMORY_PROPERTY(HOST_VISIBLE)
        PARSE_MEMORY_PROPERTY(HOST_COHERENT)
        PARSE_MEMORY_PROPERTY(HOST_CACHED)
        PARSE_MEMORY_PROPERTY(LAZILY_ALLOCATED)
        PARSE_MEMORY_PROPERTY(PROTECTED)
        
        LOGI("Vulkan MemoryType: heapIndex=%u flags=%s", memoryType.heapIndex, flags.c_str());

#undef PARSE_MEMORY_PROPERTY
    }
    
    for (uint32_t i = 0; i < memoryProperties_.memoryHeapCount; i++)
    {
        VkMemoryHeap memoryHeap = memoryProperties_.memoryHeaps[i];

#define PARSE_MEMORY_HEAP(PROPERTY)     \
    if (memoryHeap.flags & VK_MEMORY_HEAP_##PROPERTY##_BIT)    \
        flags +=  flags.empty()? #PROPERTY : std::string(" | ") + #PROPERTY;
        
        std::string flags = "";
        
        PARSE_MEMORY_HEAP(DEVICE_LOCAL)
        PARSE_MEMORY_HEAP(MULTI_INSTANCE)
        
        LOGI("Vulkan MemoryHeap: headIndex=%u size=%.03f MB flags=%s", i,
             memoryHeap.size / (1024.0f * 1024), flags.c_str());

#undef PARSE_MEMORY_HEAP
    }
}

bool VKDevice::InitDevice()
{
    RHI_ASSERT(vkInstance_ != nullptr);
    RHI_ASSERT(vkPhysicalDevice_ != nullptr);
    
    // check for vulkan info on this GPU device
    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice_, &gpuProperties);
    LOGI("Vulkan Physical Device Name: %s", gpuProperties.deviceName);
    LOGI("Vulkan Physical Device Info: apiVersion: %x \n\t driverVersion: %x",
         gpuProperties.apiVersion, gpuProperties.driverVersion);
    LOGI("API Version Supported: %d.%d.%d", VK_VERSION_MAJOR(gpuProperties.apiVersion),
         VK_VERSION_MINOR(gpuProperties.apiVersion), VK_VERSION_PATCH(gpuProperties.apiVersion));
    
    // Find a GFX queue family
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_, &queueFamilyCount, nullptr);
    RHI_ASSERT(queueFamilyCount);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_, &queueFamilyCount,
                                               queueFamilyProperties.data());
    
    uint32_t queueFamilyIndex;
    for (queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
    {
        auto flags = queueFamilyProperties[queueFamilyIndex].queueFlags;
        if ((flags & VK_QUEUE_GRAPHICS_BIT) && (flags & VK_QUEUE_COMPUTE_BIT))
        {
            break;
        }
    }
    RHI_ASSERT(queueFamilyIndex < queueFamilyCount);
    graphicQueueFamilyIndex_ = queueFamilyIndex;
    
    float priorities[] = {1.0f,};
    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.flags = 0;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.pQueuePriorities = priorities;
    
    AttemptEnumerateDeviceLayerAndExtensions();
    
    // prepare necessary extensions: Vulkan on Android need these to function
    std::vector<const char*> deviceExt;
    deviceExt.push_back("VK_KHR_swapchain");
    
    supportDebugGroup_ = HasExtension(deviceExtensions_, VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
    if (supportDebugGroup_)
    {
        deviceExt.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
    }
    
    isSupportNegativeViewport_ = HasExtension(deviceExtensions_, VK_KHR_MAINTENANCE1_EXTENSION_NAME);
    // To flip the viewport
    if (isSupportNegativeViewport_)
    {
        deviceExt.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
        gIsDeviceSupportNegativeViewport = true;
        LOGI("extension %s is enabled. use negative viewport to flip Y", VK_KHR_MAINTENANCE1_EXTENSION_NAME);
    }
    if (gIsDeviceSupportNegativeViewport != isSupportNegativeViewport_)
    {
        LOGE("gIsDeviceSupportNegativeViewport should equal isSupportNegativeViewport_");
        gIsDeviceSupportNegativeViewport = isSupportNegativeViewport_ = false;
    }
    
    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledLayerCount = deviceLayerNames_.size();
    deviceCreateInfo.ppEnabledLayerNames = deviceLayerNames_.data();
    deviceCreateInfo.enabledExtensionCount = deviceExt.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExt.data();
    
    VkPhysicalDeviceFeatures enableFeatures;
    {
        memset(&enableFeatures, 0, sizeof(enableFeatures));
        enableFeatures.multiDrawIndirect = deviceFeatures_.multiDrawIndirect;
        // enableFeatures.depthBounds = VK_TRUE;
    }
    deviceCreateInfo.pEnabledFeatures = &enableFeatures;
    
    CALL_VK(vkCreateDevice(vkPhysicalDevice_, &deviceCreateInfo, nullptr, &vkDevice_));
    
    if (VK_NULL_HANDLE == vkDevice_)
    {
        LOGE("CreateDevice fail.");
        return false;
    }

#if USE_VULKAN_MEMORY_ALLCATOR
    // init VMA
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = vkPhysicalDevice_;
    allocatorInfo.device = vkDevice_;
    allocatorInfo.instance = vkInstance_;
    allocatorInfo.vulkanApiVersion = gpuProperties.apiVersion;
    
    // VmaAllocator vmaAllocator;
    vmaCreateAllocator(&allocatorInfo, &vmaAllocator_);
    LOGI("Create VmaAllocator done.");
#endif
    
    return true;
}

bool VKDevice::InitQueueFamilies()
{
    queueFamilyIndices_.graphicsFamily = -1;
    queueFamilyIndices_.presentFamily = -1;
    queueFamilyIndices_.computeFamily = -1;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_, &queueFamilyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_, &queueFamilyCount,
                                               queueFamilies.data());
    
    for (size_t i = 0; i < queueFamilies.size(); ++i)
    {
        const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
        LOGI("VkQueueFamilyProperties i=%d queueCount=%d flags=%d", i, queueFamily.queueCount, queueFamily.queueFlags);
        
        if (queueFamily.queueCount == 0) continue;
        
        bool ok = queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        ok = ok && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT);
    
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice_, i, vkSurface_, &presentSupport);
        ok = ok && (presentSupport == VK_TRUE);
        
        if (ok)
        {
            // 要求queue同时支持present, graphic 和 compute
            queueFamilyIndices_.graphicsFamily = i;
            queueFamilyIndices_.presentFamily = i;
            queueFamilyIndices_.computeFamily = i;

            LOGI("VKDevice::InitQueueFamilies done i=%d", i);
            return true;
        }
    }
    LOGE("InitQueueFamilies fail.");
    return false;
}

VkSurfaceFormatKHR
VKDevice::ChooseVulkanSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats,
                                    VkFormat targetSurfaceFormat)
{
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        return {targetSurfaceFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == targetSurfaceFormat &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR
VKDevice::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
        else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            bestMode = availablePresentMode;
        }
    }
    return bestMode;
}

Extent2D VKDevice::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return {capabilities.currentExtent.width, capabilities.currentExtent.height};
    }
    else
    {
        Extent2D actualExtent = {4096, 4096};
        actualExtent.width = std::max(capabilities.minImageExtent.width,
                                      std::min(capabilities.maxImageExtent.width,
                                               actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height,
                                                actualExtent.height));
        return actualExtent;
    }
}

SwapChainSupportDetails VKDevice::QuerySwapChainSupport(VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;
    // Capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice_, surface, &details.capabilities);
    //formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice_, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice_, surface, &formatCount,
                                               details.formats.data());
    }
    // presentMode
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice_, surface, &presentModeCount,
                                                nullptr);
    
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice_, surface, &presentModeCount,
                                                    details.presentModes.data());
    }
    return details;
}

bool VKDevice::IsDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    
    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}

uint32_t VKDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    for (uint32_t i = 0; i < memoryProperties_.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) &&
            (memoryProperties_.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    
    LOGE("failed to find suitable memory type!");
    return -1;
}

bool VKDevice::InitCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = (std::uint32_t)queueFamilyIndices_.graphicsFamily,
    };
    
    CALL_VK(vkCreateCommandPool(vkDevice_, &poolInfo, nullptr, &vkCommandPool_));
    if (VK_NULL_HANDLE == vkCommandPool_)
    {
        LOGE("CreateCommandPool fail.");
    }
    LOGI("InitCommandPool done.");
    return VK_NULL_HANDLE != vkCommandPool_;
}

bool VKDevice::InitDescriptorPool()
{
    const std::uint32_t descriptorCountInPool = 1024;
    const std::uint32_t maxSets = 1024;
    
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (int i = 0; i < maxSets / descriptorCountInPool; ++i)
    {
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCountInPool});
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCountInPool});
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_SAMPLER, descriptorCountInPool});
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptorCountInPool});
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, descriptorCountInPool});
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptorCountInPool});
    }
    
    VkDescriptorPoolCreateInfo poolInfo;
    {
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.pNext = nullptr;
        poolInfo.flags = 0;
        poolInfo.poolSizeCount = (std::uint32_t)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = maxSets;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    }
    
    CALL_VK(vkCreateDescriptorPool(vkDevice_, &poolInfo, nullptr, &vkDescriptorPool_));
    
    if (VK_NULL_HANDLE == vkDescriptorPool_)
    {
        LOGE("CreateDescriptorPool fail.");
    }
    return VK_NULL_HANDLE != vkDescriptorPool_;
}

RenderBundleEncoder* VKDevice::CreateRenderBundleEncoder(
    const RenderBundleEncoderDescriptor &descriptor)
{
    return CreateObject<VKRenderBundleEncoder>(descriptor);
}

RenderPipeline* VKDevice::CreateRenderPipeline(RenderPipelineDescriptor &descriptor)
{
    for (size_t i = 0; i < renderPipelineCache_.size(); ++i)
    {
        if (renderPipelineCache_[i].first == descriptor)
        {
            return renderPipelineCache_[i].second;
        }
    }
    
    auto pipeline = CreateObject<VKRenderPipeline>(descriptor);
    renderPipelineCache_.emplace_back(descriptor, pipeline);
    RHI_SAFE_RETAIN(pipeline);
    
    return pipeline;
}

ComputePipeline* VKDevice::CreateComputePipeline(ComputePipelineDescriptor &descriptor)
{
    return CreateObject<VKComputePipeline>(descriptor);
}

Shader* VKDevice::CreateShaderModule(const ShaderModuleDescriptor &descriptor)
{
    return CreateObject<VKShader>(descriptor);
}

Texture* VKDevice::CreateTexture(const TextureDescriptor &descriptor)
{
    auto texture = CreateObject<VKTexture>(descriptor);
    if (texture)
    {
        GetObjectManager().increaseTextureMem(texture->getMemoryUsage());
    }
    return texture;
}

Sampler* VKDevice::CreateSampler(const SamplerDescriptor &descriptor)
{
    return CreateObject<VKSampler>(descriptor);
}

SamplerBinding* VKDevice::CreateSamplerBinding(Sampler* sampler)
{
    return CreateObject<SamplerBinding>(sampler);
}

TextureViewBinding* VKDevice::CreateTextureViewBinding(TextureView* view)
{
    return CreateObject<TextureViewBinding>(view);
}

BindGroupLayout* VKDevice::CreateBindGroupLayout(const BindGroupLayoutDescriptor &descriptor)
{
    for (size_t i = 0; i < bindGroupLayoutCache_.size(); ++i)
    {
        if (bindGroupLayoutCache_[i].first == descriptor)
        {
            return bindGroupLayoutCache_[i].second;
        }
    }
    
    auto bindGroupLayout = CreateObject<VKBindGroupLayout>(descriptor);
    bindGroupLayoutCache_.emplace_back(descriptor, bindGroupLayout);
    RHI_SAFE_RETAIN(bindGroupLayout);
    
    return bindGroupLayout;
}

BindGroup* VKDevice::CreateBindGroup(BindGroupDescriptor &descriptor)
{
    return CreateObject<VKBindGroup>(descriptor);
}

PipelineLayout*
VKDevice::CreatePipelineLayout(const PipelineLayoutDescriptor &descriptor)
{
    for (size_t i = 0; i < pipelineLayoutCache_.size(); ++i)
    {
        if (pipelineLayoutCache_[i].first == descriptor)
        {
            return pipelineLayoutCache_[i].second;
        }
    }
    
    auto pipelineLayout = CreateObject<VKPipelineLayout>(descriptor);
    pipelineLayoutCache_.emplace_back(descriptor, pipelineLayout);
    RHI_SAFE_RETAIN(pipelineLayout);
    
    return pipelineLayout;
}

Queue* VKDevice::CreateQueue()
{
    return CreateObject<VKQueue>();
}

CommandEncoder* VKDevice::CreateCommandEncoder(const CommandEncoderDescriptor &descriptor)
{
    return CreateObject<VKCommandEncoder>();
}

Fence* VKDevice::CreateFence(const FenceDescriptor &descriptor)
{
    return CreateObject<VKFence>(descriptor);
}

QuerySet* VKDevice::CreateQuerySet(const QuerySetDescriptor &descriptor)
{
    return CreateObject<VKQuerySet>(descriptor);
}

Queue* VKDevice::GetQueue()
{
    if (!renderQueue_)
    {
        RHI_PTR_ASSIGN(renderQueue_, CreateQueue());
    }
    return renderQueue_;
}

VKCommandBuffer* VKDevice::CreateCommandBuffer()
{
    auto obj = GetObjectManager().GetObjectFromCacheByType(RHIObjectType::CommandBuffer);
    if (!obj)
    {
        obj = CreateObject<VKCommandBuffer>();
    }
    return RHI_CAST(VKCommandBuffer*, obj);
}

VKTextureView* VKDevice::CreateTextureView(VKTexture* vkTexture,
                                           const TextureViewDescriptor &descriptor)
{
    return pTextureViewMgr_->GetOrCreateTextureView(vkTexture, descriptor);
}

bool
VKDevice::HasExtension(const std::vector<VkExtensionProperties> &extensions, const char* extension)
{
    bool found = false;
    for (uint32_t i = 0; i < extensions.size() && !found; i++)
    {
        found = strcmp(extensions[i].extensionName, extension) == 0;
    }
    return found;
}

void VKDevice::InvalidateFramebuffers()
{
    SCOPED_LOCK(mutex_);
    for (auto fb : framebufferCache_)
    {
        fb.second->Invalidate();
    }
}

VkSemaphore VKDevice::AcquireVkSemaphore()
{
    const static VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                                        nullptr, 0};
    VkSemaphore semaphore;
    CALL_VK(vkCreateSemaphore(GetNative(), &semaphoreInfo, nullptr, &semaphore));
    return semaphore;
}

void VKDevice::ReturnVkSemaphore(VkSemaphore vkSemaphore)
{
    // TODO reuse the semaphore
    vkDestroySemaphore(GetNative(), vkSemaphore, nullptr);
}

VkFence VKDevice::AcquireVkFence()
{
    if (vkFenceCache_.empty())
    {
        const static VkFenceCreateInfo fenceCreateInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                                          nullptr, 0};
        VkFence fence;
        CALL_VK(vkCreateFence(GetNative(), &fenceCreateInfo, nullptr, &fence));
        vkFenceCache_.push_back(fence);
    }
    auto fence = vkFenceCache_.back();
    vkFenceCache_.pop_back();
    return fence;
}

void VKDevice::ReturnVkFence(VkFence vkFence)
{
    vkResetFences(GetNative(), 1, &vkFence);
    vkFenceCache_.push_back(vkFence);
}

CommandListPtr VKDevice::GetCommandList()
{
    if (freeCommandLists_.empty())
    {
        freeCommandLists_.push_back(
            static_cast<CommandListPtr>(std::make_shared<CommandList>()));
    }
    auto cmdList = freeCommandLists_.back();
    freeCommandLists_.pop_back();
    return cmdList;
}

void VKDevice::ReleaseCommandList(CommandListPtr pCommandList)
{
    SCOPED_LOCK(mutex_);
    if (pCommandList)
    {
        pendingReleaseCommandLists_.push_back(pCommandList);
        pCommandList.reset();
    }
    else
    {
        LOGE("invalid commandList");
    }
}

void VKDevice::ScheduleCallbackExecutedInGameThread(const std::function<void(VKDevice*)> &callback)
{
    SCOPED_LOCK(mutexScheduleAsyncCallback_);
    scheduledAsyncCallbacks_.push_back(callback);
}

// define

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
    VkDebugReportFlagsEXT msgFlags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t srcObject, size_t location,
    int32_t msgCode, const char* pLayerPrefix,
    const char* pMsg, void* pUserData)
{
    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        LOGE("ERROR: [%s] Code %i : %s", pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        LOGW("WARNING: [%s] Code %i : %s", pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        LOGW("PERFORMANCE WARNING: [%s] Code %i : %s", pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        LOGI("INFO: [%s] Code %i : %s", pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        LOGI("DEBUG: [%s] Code %i : %s", pLayerPrefix, msgCode, pMsg);
    }
    return VK_FALSE;
}

void DumpVulkanErrorMessage(VkResult code, const char* filename, uint32_t lineno)
{
    if (VK_SUCCESS == code)
        return;
    
    const char* message = "UNDEFINED";
    switch (code)
    {
        case VK_SUCCESS:
            message = "VK_SUCCESS";
            break;
        case VK_NOT_READY:
            message = "VK_NOT_READY";
            break;
        case VK_TIMEOUT:
            message = "VK_TIMEOUT";
            break;
        case VK_EVENT_SET:
            message = "VK_EVENT_SET";
            break;
        case VK_EVENT_RESET:
            message = "VK_EVENT_RESET";
            break;
        case VK_INCOMPLETE:
            message = "VK_INCOMPLETE";
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            message = "VK_ERROR_OUT_OF_HOST_MEMORY";
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            message = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
            break;
        case VK_ERROR_INITIALIZATION_FAILED:
            message = "VK_ERROR_INITIALIZATION_FAILED";
            break;
        case VK_ERROR_DEVICE_LOST:
            message = "VK_ERROR_DEVICE_LOST";
            break;
        case VK_ERROR_MEMORY_MAP_FAILED:
            message = "VK_ERROR_MEMORY_MAP_FAILED";
            break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            message = "VK_ERROR_LAYER_NOT_PRESENT";
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            message = "VK_ERROR_EXTENSION_NOT_PRESENT";
            break;
        case VK_ERROR_FEATURE_NOT_PRESENT:
            message = "VK_ERROR_FEATURE_NOT_PRESENT";
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            message = "VK_ERROR_INCOMPATIBLE_DRIVER";
            break;
        case VK_ERROR_TOO_MANY_OBJECTS:
            message = "VK_ERROR_TOO_MANY_OBJECTS";
            break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            message = "VK_ERROR_FORMAT_NOT_SUPPORTED";
            break;
        case VK_ERROR_FRAGMENTED_POOL:
            message = "VK_ERROR_FRAGMENTED_POOL";
            break;
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            message = "VK_ERROR_OUT_OF_POOL_MEMORY";
            break;
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            message = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
            break;
        case VK_ERROR_SURFACE_LOST_KHR:
            message = "VK_ERROR_SURFACE_LOST_KHR";
            break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            message = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            message = "VK_ERROR_OUT_OF_DATE_KHR";
            break;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            message = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
            break;
        case VK_ERROR_VALIDATION_FAILED_EXT:
            message = "VK_ERROR_VALIDATION_FAILED_EXT";
            break;
        case VK_ERROR_INVALID_SHADER_NV:
            message = "VK_ERROR_INVALID_SHADER_NV";
            break;
        case VK_ERROR_FRAGMENTATION_EXT:
            message = "VK_ERROR_FRAGMENTATION_EXT";
            break;
        case VK_ERROR_NOT_PERMITTED_EXT:
            message = "VK_ERROR_NOT_PERMITTED_EXT";
            break;
        default:
            break;
    }
    LOGE("Vulkan error [%s-%d]. File[%s], line[%d] \n", message, code, filename, lineno);
}

NS_RHI_END
