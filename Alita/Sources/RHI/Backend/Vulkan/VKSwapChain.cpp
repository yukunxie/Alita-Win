//
// Created by realxie on 2019-10-21.
//

#include "VKSwapChain.h"
#include "VKQueue.h"
#include "VKTexture.h"
#include "VKTextureView.h"
#include "VKTextureViewManager.h"
#include "RenderThreading.h"
#include "VKCommandBuffer.h"

#include <chrono>


NS_GFX_BEGIN

VKSwapChain::VKSwapChain(VKDevice* device)
    : SwapChain(device)
{
}

VKSwapChain::~VKSwapChain()
{
    if (VKDEVICE()->GetSwapChain() == this)
    {
        VKDEVICE()->SetSwapChain(nullptr);
    }
    
    Dispose();
    
    pSemaphoreCxx_.reset();
    
    // TODO realxie 这里需要仔细处理一下commandbuffer的所有权问题
    for (auto cmdBuffer : renderingFrameInfo_.commandBuffers) {
        GFX_SAFE_RELEASE(cmdBuffer);
    }
    renderingFrameInfo_.commandBuffers.clear();
}

bool VKSwapChain::Init(const SwapChainDescriptor& descriptor)
{
    format_ = VKDEVICE()->GetSwapchainPreferredFormat();
    textureUsage_ = descriptor.usage | TextureUsage::PRESENT;
    vkUsages_ |= GetVkImageUsageFlags(textureUsage_);
    
    CreateVulkanSwapChain();
    
    Init();
    
    VKDEVICE()->SetSwapChain(this);
    
    for (ssize_t i = 0; i < swapchainTextures_.size(); ++i)
    {
        auto &frame = frameResources_[i];
        frame.swapChain = this;
        frame.imageIndex = i;
        frame.fenceToSyncSubmission = VKDEVICE()->AcquireVkFence();
        frame.semaImageAvaliable = VKDEVICE()->AcquireVkSemaphore();
        frame.semaPresentable = VKDEVICE()->AcquireVkSemaphore();
    }
    
    return true;
}

void VKSwapChain::CreateVulkanSwapChain()
{
    LOGI("CreateVulkanSwapChain");
    auto queueFamilyIndices_ = VKDEVICE()->GetQueueFamilyIndices();
    auto vkSurface_ = VKDEVICE()->GetVulkanSurface();
    
    std::uint32_t queueFamilyIndices[] = {
        (std::uint32_t) queueFamilyIndices_.graphicsFamily,
        (std::uint32_t) queueFamilyIndices_.presentFamily
    };
    
    const DefaultSwapchainConfig &config = VKDEVICE()->GetSwapchainConfig();
    LOGI("CreateVulkanSwapChain imageCount=%d", config.imageCount);
    
    vkSwapchainImageFormat_ = config.format;
    extent_ = config.extent2D;
    
    if (!pSemaphoreCxx_)
    {
        pSemaphoreCxx_ = std::make_unique<SemaphoreCXX>(config.imageCount);
    }
    
    VkSwapchainCreateInfoKHR createInfo;
    {
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        createInfo.surface = vkSurface_;
        createInfo.minImageCount = config.imageCount;
        createInfo.imageFormat = vkSwapchainImageFormat_;
        createInfo.imageColorSpace = config.colorSpaceKhr;
        createInfo.imageExtent = {extent_.width, extent_.height};
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vkUsages_;
        createInfo.preTransform = config.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
    }
    
    if (queueFamilyIndices_.graphicsFamily != queueFamilyIndices_.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional   createInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    
    // create swapchain.
    CALL_VK(vkCreateSwapchainKHR(GFX_CAST(VKDevice * , GetGPUDevice())->GetNative(), &createInfo,
                                   nullptr, &vkSwapChain_));
}

void VKSwapChain::Init()
{
    auto vkDevice = GFX_CAST(VKDevice*, GetGPUDevice())->GetNative();
    
    std::vector<VkImage> swapChainImages;
    
    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain_, &imageCount_, nullptr);
    swapChainImages.resize(imageCount_);
    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain_, &imageCount_, swapChainImages.data());
    
    GFX_ASSERT(swapchainTextures_.size() <= swapChainImages.size());
    if (swapchainTextures_.size() != swapChainImages.size())
    {
        swapchainTextures_.resize(swapChainImages.size());
    }
    
    for (size_t i = 0; i < swapChainImages.size(); ++i)
    {
        if (swapchainTextures_[i])
        {
            // Here. this branch is adapted for recreate swapchain.
            swapchainTextures_[i]->SetVkImageHandleDirectly(swapChainImages[i]);
            swapchainTextures_[i]->textureSize_ = { extent_.width, extent_.height, 1 };
        }
        else
        {
            TextureDescriptor descriptor;
            descriptor.usage = textureUsage_;
            descriptor.size = {extent_.width, extent_.height, 1};
            descriptor.sampleCount = 1;
            descriptor.dimension = TextureDimension::TEXTURE_2D;
            descriptor.arrayLayerCount = 1;
            descriptor.format = format_;
            descriptor.mipLevelCount = 1;
            swapchainTextures_[i] = VKDEVICE()->CreateObject<VKTexture>(swapChainImages[i],
                                                                        descriptor);
            swapchainTextures_[i]->MarkSwapchainImage();
            swapchainTextures_[i]->SetIsSwapChainTexture(true);
            GFX_SAFE_RETAIN(swapchainTextures_[i]);
        }
    }
}

void VKSwapChain::Dispose()
{
    for (ssize_t i = 0; i < swapchainTextures_.size(); ++i)
    {
        auto &frame = frameResources_[i];
        VKDEVICE()->ReturnVkFence(frame.fenceToSyncSubmission);
        VKDEVICE()->ReturnVkSemaphore(frame.semaImageAvaliable);
        VKDEVICE()->ReturnVkSemaphore(frame.semaPresentable);
    }
    
    auto textureViewCache = VKDEVICE()->GetTextureViewManager();
    for (auto texture : swapchainTextures_)
    {
        if (textureViewCache)
        {
            textureViewCache->RemoveByTexture(texture);
        }
        GFX_SAFE_RELEASE(texture);
    }
    swapchainTextures_.clear();
    
    if (vkSwapChain_)
    {
        vkDestroySwapchainKHR(GFX_CAST(VKDevice*, GetGPUDevice())->GetNative(), vkSwapChain_,
                                nullptr);
        vkSwapChain_ = VK_NULL_HANDLE;
    }
}

void VKSwapChain::NotifyPresentDone(bool hasDrawCommands)
{
    pSemaphoreCxx_->Signal();
    
    VKDEVICE()->ScheduleCallbackExecutedInGameThread([hasDrawCommands](VKDevice* device) {
        device->SetPresentDone(hasDrawCommands);
    });
}

bool VKSwapChain::RecreateSwapChainInternal()
{
    LOGW("Try to create swap chain.");
    
    // Must in the sumbit thread
    VKDEVICE()->GetAsyncWorker()->CheckThread();
    VKDEVICE()->CheckOnBackgroud();
    
    // Release vulkan image resource manually.
    for (auto texture : swapchainTextures_)
    {
        texture->DisposeNativeHandle();
    }
    
    if (vkSwapChain_)
    {
        vkDestroySwapchainKHR(GFX_CAST(VKDevice*, GetGPUDevice())->GetNative(), vkSwapChain_,
                                nullptr);
        vkSwapChain_ = VK_NULL_HANDLE;
    }
    
    vkDeviceWaitIdle(VKDEVICE()->GetNative());
    
    if (!VKDEVICE()->CreateSurface())
    {
        LOGE("Try to create swap chain fail.");
        return false;
    }
    
    CreateVulkanSwapChain();
    Init();
    
    VKDEVICE()->InvalidateFramebuffers();
    
    return true;
}

void VKSwapChain::RecreateSwapChain()
{
    while (!RecreateSwapChainInternal())
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}

void VKSwapChain::AcquireNextImage()
{
    pSemaphoreCxx_->Wait();
    imageIndex_ = (imageIndex_ + 1) % swapchainTextures_.size();
}

Texture* VKSwapChain::GetCurrentTexture()
{
    return swapchainTextures_[imageIndex_];
}

bool VKSwapChain::AcquireNextImageKHR(VkSemaphore semaphore, std::uint32_t* pImageIndex)
{
    VkResult code = vkAcquireNextImageKHR(VKDEVICE()->GetNative(),
                                            this->GetNative(),
                                            kMaxUint64,
                                            semaphore,
                                            VK_NULL_HANDLE,
                                            pImageIndex);
    
    if (VK_SUCCESS == code || VK_TIMEOUT == code || VK_NOT_READY == code || VK_SUBOPTIMAL_KHR == code)
    {
        if (code != VK_SUCCESS)
            LOGW("vkAcquireNextImageKHR code=%s", GetVkResultString(code));
        return true;
    }
    
    LOGE("vkAcquireNextImageKHR fail code=%s", GetVkResultString(code));
    
    if (VK_ERROR_SURFACE_LOST_KHR == code || VK_ERROR_OUT_OF_DATE_KHR == code)
    {
        RecreateSwapChain();
        AcquireNextImageKHR(semaphore, pImageIndex);
    }
    
    // 重建的swapchain，返回false
    return false;
}

void VKSwapChain::Present(std::uint32_t imageIndex, VkSemaphore semaWaiting, bool hasDrawCommands)
{
    VkResult result;
    VkPresentInfoKHR presentInfo;
    {
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &vkSwapChain_;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &semaWaiting;
        presentInfo.pResults = &result;
    }
    
    VKQueue* queue = GFX_CAST(VKQueue*, VKDEVICE()->GetQueue());
    
    VkResult code = vkQueuePresentKHR(queue->GetNative(), &presentInfo);
    if (code != VK_SUCCESS || result != VK_SUCCESS)
    {
        LOGE("QueuePresentKHR fail, code=%s result=%s", GetVkResultString(code),
             GetVkResultString(result));
    }

    // todo;
    vkQueueWaitIdle(queue->GetNative());
    
    if (code == VK_ERROR_OUT_OF_DATE_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
    }
    
    NotifyPresentDone(hasDrawCommands);
}
NS_GFX_END
