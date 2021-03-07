//
// Created by realxie on 2019-10-02.
//

#include "VKDevice.h"
#include "VulkanMarcos.h"
#include "VKBuffer.h"
#include "VKRenderPipeline.h"
#include "VKShader.h"
#include "VKRenderPass.h"
#include "VKTexture.h"
#include "VKSampler.h"
#include "VKTextureView.h"
#include "VKPipelineLayout.h"
#include "VKBindGroupLayout.h"
#include "VKBindGroup.h"
#include "VKQueue.h"
#include "VKRenderQueue.h"
#include "VKCommandEncoder.h"
#include "VKSwapChain.h"
#include "../../Include/xxhash64.h"

#include <vector>
#include <array>
#include <stdexcept>

NS_RHI_BEGIN

XXHash64 gXXHash64Generator(0x21378732);

PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
    VkDebugReportFlagsEXT msgFlags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t srcObject, size_t location,
    int32_t msgCode, const char* pLayerPrefix,
    const char* pMsg, void* pUserData)
{
    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        LOGE("ERROR: [%s] Code %i : %s",
                            pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        LOGW("WARNING: [%s] Code %i : %s",
                            pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        LOGW("PERFORMANCE WARNING: [%s] Code %i : %s",
                            pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        LOGI("INFO: [%s] Code %i : %s",
                            pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        LOGI("DEBUG: [%s] Code %i : %s",
                            pLayerPrefix, msgCode, pMsg);
    }
    
    // Returning false tells the layer not to stop when the event occurs, so
    // they see the same behavior with and without validation layers enabled.
    return VK_FALSE;
}

#ifdef ANDROID
VKDevice::VKDevice(ANativeWindow* window)
#elif defined(WIN32)
VKDevice::VKDevice(GLFWwindow* window)
#endif
{
#ifdef ANDROID
    // load Vulkan interface from so.
    InitVulkan();
#endif
    
    nativeWindow_ = window;
    
    CreateInstance();
    CreateSurface();
    CreatePhysicDevice();
    CreateDevice();
    queueFamilyIndices_ = FindQueueFamilies();
//    CreateSwapchain();
    CreateVKQueue();
    CreateCommandPool();
    CreateDescriptorPool();
}

VKDevice::~VKDevice()
{
    // TODO free vulkan resource
    
    if (vkDestroyDebugReportCallbackEXT)
    {
        vkDestroyDebugReportCallbackEXT(vkInstance_, vkDebugReportCallback_, nullptr);
    }
}

RenderPass* VKDevice::GetOrCreateRenderPass(const RenderPassCacheQuery &query)
{
    auto it = renderPassCache_.find(query);
    if (it != renderPassCache_.end())
    {
        RHI_SAFE_RETAIN(it->second);
        return it->second;
    }
    auto renderPass = new VKRenderPass(this, query);
    renderPassCache_[query] = renderPass;
    RHI_SAFE_RETAIN(renderPass);
    
    // retain before release
    RHI_SAFE_RETAIN(renderPass);
    return renderPass;
}

Buffer* VKDevice::CreateBuffer(const BufferDescriptor &descriptor)
{
    Buffer* buffer = new VKBuffer(this, descriptor);
    RHI_SAFE_RETAIN(buffer);
    return buffer;
}

void VKDevice::WriteBuffer(const Buffer* buffer, const void* data, std::uint32_t offset,
                           std::uint32_t size)
{
    ((VKBuffer*) buffer)->UpdateBuffer(data, offset, size);
}

// private

void VKDevice::CreateInstance()
{
    // Get layer count using null pointer as last parameter
    uint32_t instance_layer_present_count = 0;
    vkEnumerateInstanceLayerProperties(&instance_layer_present_count, nullptr);

    std::vector<VkLayerProperties> layer_props(instance_layer_present_count);
    
    // Enumerate layers with valid pointer in last parameter
    //VkLayerProperties* layer_props = (VkLayerProperties*) malloc(
        //instance_layer_present_count * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&instance_layer_present_count, layer_props.data());
    
    // Make sure the desired validation layer is available
    std::vector<const char*> targetLayers = {
        "VK_LAYER_GOOGLE_threading",
        "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_core_validation",
        "VK_LAYER_GOOGLE_unique_objects",
        "VK_LAYER_KHRONOS_validation",
    };

    std::vector<const char*> validLayers;

    for (const char* name : targetLayers)
    {
        for (const VkLayerProperties& p : layer_props)
        {
            if (strcmp(p.layerName, name) == 0)
            {
                validLayers.push_back(name);
                break;
            }
        }
    }

    
    /*uint32_t instance_layer_request_count = sizeof(instance_layers) / sizeof(instance_layers[0]);
    for (uint32_t i = 0; i < instance_layer_request_count; i++)
    {
        bool found = false;
        for (uint32_t j = 0; j < instance_layer_present_count; j++)
        {
            if (strcmp(instance_layers[i], layer_props[j].layerName) == 0)
            {
                found = true;
            }
        }
        if (!found)
        {
            Assert(false);
        }
    }*/
    
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Alita",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "RXEngine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_MAKE_VERSION(1, 0, 0)

    };
    
    // prepare necessary extensions: Vulkan on Android need these to function
    std::vector<const char*> instanceExt;
    instanceExt.push_back("VK_KHR_surface");
#ifdef ANDROID
    instanceExt.push_back("VK_KHR_android_surface");
#endif
    const char* DEBUG_REPORT_EXTENSION = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
    
    // Enable Vulkan debug callback.
    {
        // Get the instance extension count
        uint32_t inst_ext_count = 0;
        CALL_VK(vkEnumerateInstanceExtensionProperties(nullptr, &inst_ext_count, nullptr));
        
        // Enumerate the instance extensions
        VkExtensionProperties* inst_exts =
            (VkExtensionProperties*) malloc(inst_ext_count * sizeof(VkExtensionProperties));
        vkEnumerateInstanceExtensionProperties(nullptr, &inst_ext_count, inst_exts);
        
        const char* enabled_inst_exts[16] = {};
        uint32_t enabled_inst_ext_count = 0;
        
        // Make sure the debug report extension is available
        for (uint32_t i = 0; i < inst_ext_count; i++)
        {
            if (strcmp(inst_exts[i].extensionName,
                       VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0)
            {
                enabled_inst_exts[enabled_inst_ext_count++] =
                    VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
            }
        }
        
        if (enabled_inst_ext_count > 0)
        {
            //instanceExt.push_back(DEBUG_REPORT_EXTENSION);
        }
    }


    
    // Create the Vulkan instance
    VkInstanceCreateInfo instanceCreateInfo;
    {
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledExtensionCount = 0;// static_cast<uint32_t>(instanceExt.size());
        instanceCreateInfo.ppEnabledExtensionNames = instanceExt.data();
        instanceCreateInfo.enabledLayerCount = validLayers.size();//insance_layer_request_count;
        instanceCreateInfo.ppEnabledLayerNames = validLayers.data();
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
    instanceCreateInfo.enabledExtensionCount = strExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = strExtensions.data();
#endif
    
    CALL_VK(vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance_));
    
    if (!vkCreateDebugReportCallbackEXT)
    {
        vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)
            vkGetInstanceProcAddr(vkInstance_, "vkCreateDebugReportCallbackEXT");
        vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)
            vkGetInstanceProcAddr(vkInstance_, "vkDestroyDebugReportCallbackEXT");
    }
    
    // Create the debug callback with desired settings
    if (vkCreateDebugReportCallbackEXT)
    {
        
        VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo;
        debugReportCallbackCreateInfo.sType =
            VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debugReportCallbackCreateInfo.pNext = NULL;
        debugReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                              VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                              VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debugReportCallbackCreateInfo.pfnCallback = DebugReportCallback;
        debugReportCallbackCreateInfo.pUserData = NULL;
        
        CALL_VK(vkCreateDebugReportCallbackEXT(vkInstance_, &debugReportCallbackCreateInfo,
                                               nullptr, &vkDebugReportCallback_));
    }
    
    
}

void VKDevice::CreateSurface()
{
    RHI_ASSERT(nativeWindow_ != nullptr);
    RHI_ASSERT(vkInstance_ != nullptr);
    
#ifdef WIN32
    CALL_VK(glfwCreateWindowSurface(vkInstance_, nativeWindow_, NULL, &vkSurface_));
#elif defined(ANDROID)
    // if we create a surface, we need the surface extension
    VkAndroidSurfaceCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .window = nativeWindow_
    };
    vkCreateAndroidSurfaceKHR(vkInstance_, &createInfo, nullptr, &vkSurface_);
#endif
}

void VKDevice::CreatePhysicDevice()
{
    RHI_ASSERT(vkInstance_ != nullptr);
    
    uint32_t gpuCount = 0;
    CALL_VK(vkEnumeratePhysicalDevices(vkInstance_, &gpuCount, nullptr));
    VkPhysicalDevice tmpGpus[10] = {nullptr};
    CALL_VK(vkEnumeratePhysicalDevices(vkInstance_, &gpuCount, tmpGpus));
    
    // Pick up the first GPU Devicereturn true;
    vkPhysicalDevice_ = tmpGpus[0];
}

void VKDevice::CreateDevice()
{
    RHI_ASSERT(vkInstance_ != nullptr);
    RHI_ASSERT(vkPhysicalDevice_ != nullptr);
    RHI_ASSERT(vkPhysicalDevice_ != nullptr);
    
    // check for vulkan info on this GPU device
    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice_, &gpuProperties);
    LOGI("Vulkan Physical Device Name: %s", gpuProperties.deviceName);
    LOGI("Vulkan Physical Device Info: apiVersion: %x \n\t driverVersion: %x",
         gpuProperties.apiVersion, gpuProperties.driverVersion);
    LOGI("API Version Supported: %d.%d.%d", VK_VERSION_MAJOR(gpuProperties.apiVersion),
         VK_VERSION_MINOR(gpuProperties.apiVersion), VK_VERSION_PATCH(gpuProperties.apiVersion));
    
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice_, vkSurface_, &surfaceCapabilities);
    
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
        if (queueFamilyProperties[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            break;
        }
    }
    RHI_ASSERT(queueFamilyIndex < queueFamilyCount);
    graphicQueueFamilyIndex_ = queueFamilyIndex;
    
    float priorities[] = {1.0f,};
    VkDeviceQueueCreateInfo queueCreateInfo;
    {
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.flags = 0;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.pQueuePriorities = priorities;
    }
    
    // prepare necessary extensions: Vulkan on Android need these to function
    std::vector<const char*> deviceExt;
    deviceExt.push_back("VK_KHR_swapchain");
    
    VkDeviceCreateInfo deviceCreateInfo;
    {
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = nullptr;
        deviceCreateInfo.flags = 0;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.ppEnabledLayerNames = nullptr;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExt.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExt.data();
        deviceCreateInfo.pEnabledFeatures = nullptr; 
    }
    
    CALL_VK(vkCreateDevice(vkPhysicalDevice_, &deviceCreateInfo, nullptr, &vkDevice_));
    
    RHI_ASSERT(vkDevice_ != nullptr);
}

QueueFamilyIndices VKDevice::FindQueueFamilies()
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_, &queueFamilyCount,
                                             queueFamilies.data());
    
    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }
        
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice_, i, vkSurface_, &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport)
        {
            indices.presentFamily = i;
        }
        
        if (indices.isComplete())
        {
            break;
        }
        i++;
    }
    
    return indices;
}

VkSurfaceFormatKHR
VKDevice::ChooseVulkanSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats, VkFormat targetSurfaceFormat)
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
        vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice_, surface, &formatCount, details.formats.data());
    }
    // presentMode
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice_, surface, &presentModeCount, nullptr);
    
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
    VkPhysicalDeviceFeatures deviceFeatures;
    
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    
    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}

uint32_t VKDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice_, &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    
    throw std::runtime_error("failed to find suitable memory type!");
}


void VKDevice::CreateVKQueue()
{
//    vkGetDeviceQueue(vkDevice_, queueFamilyIndices_.presentFamily, 0, &vkQueue_);
    renderQueuer_ = CreateQueue();
}

void VKDevice::CreateCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = 0
    };
    
    CALL_VK(vkCreateCommandPool(vkDevice_, &poolInfo, nullptr, &vkCommandPool_));
}

void VKDevice::CreateDescriptorPool()
{
    uint32_t masSets = 1024 * 1024;
    
    std::vector<VkDescriptorPoolSize> poolSizes = {
        VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1024, 
        },
        VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1024,
        },
        VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = 1024,
        },
    };
    
    VkDescriptorPoolCreateInfo poolInfo;
    {
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.pNext = nullptr;
        poolInfo.flags = 0;
        poolInfo.poolSizeCount = (std::uint32_t)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = masSets;
    };
    
    CALL_VK(vkCreateDescriptorPool(vkDevice_, &poolInfo, nullptr, &vkDescriptorPool_));
}

RenderPipeline* VKDevice::CreateRenderPipeline(const RenderPipelineDescriptor &descriptor)
{
    RenderPipeline* pipeline = new VKRenderPipeline(this, descriptor);
    RHI_SAFE_RETAIN(pipeline);
    return pipeline;
}

Shader* VKDevice::CreateShaderModule(const ShaderModuleDescriptor &descriptor)
{
    return VKShader::Create(this, descriptor);
}

Texture* VKDevice::CreateTexture(const TextureDescriptor &descriptor)
{
    auto* texture = new VKTexture();
    RHI_SAFE_RETAIN(texture);
    if (texture->Init(this, descriptor))
    {
        return texture;
    }
    RHI_SAFE_RELEASE(texture);
    return nullptr;
}

Sampler* VKDevice::CreateSampler(const SamplerDescriptor &descriptor)
{
    VKSampler* sampler = new VKSampler();
    if (sampler->Init(this, descriptor))
    {
        RHI_SAFE_RETAIN(sampler);
        return sampler;
    }
    else
    {
        delete sampler;
        return nullptr;
    }
}

BindGroupLayout* VKDevice::CreateBindGroupLayout(const BindGroupLayoutDescriptor &descriptor)
{
    return VKBindGroupLayout::Create(this, descriptor);
}

BindGroup* VKDevice::CreateBindGroup(const BindGroupDescriptor& descriptor)
{
    return VKBindGroup::Create(this, descriptor);
}

PipelineLayout*
VKDevice::CreatePipelineLayout(const PipelineLayoutDescriptor& descriptor)
{
    return VKPipelineLayout::Create(this, descriptor);
}

Queue* VKDevice::CreateQueue()
{
    VKQueue* renderQueue = new VKQueue(this);
    RHI_SAFE_RETAIN(renderQueue);
    return renderQueue;
}

CommandEncoder* VKDevice::CreateCommandEncoder(const CommandEncoderDescriptor& descriptor)
{
    VKCommandEncoder* commandEncoder = new VKCommandEncoder(this);
    RHI_SAFE_RETAIN(commandEncoder);
    return commandEncoder;
}

void
VKDevice::SetImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldImageLayout,
                         VkImageLayout newImageLayout, VkPipelineStageFlags srcStages,
                         VkPipelineStageFlags destStages)
{
    VkImageMemoryBarrier imageMemoryBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = 0,
        .dstAccessMask = 0,
        .oldLayout = oldImageLayout,
        .newLayout = newImageLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };
    
    switch (oldImageLayout)
    {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;
        
        default:
            break;
    }
    
    switch (newImageLayout)
    {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
        
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.subresourceRange.aspectMask =
                VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                               VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destStages = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            break;
        
        default:
            break;
    }
    
    vkCmdPipelineBarrier(cmdBuffer, srcStages, destStages, 0, 0, NULL, 0, NULL, 1,
                         &imageMemoryBarrier);
}

Queue* VKDevice::GetQueue() const
{
    RHI_ASSERT(renderQueuer_ != nullptr);
    return renderQueuer_;
}

NS_RHI_END
