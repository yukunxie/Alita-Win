//
// Created by realxie on 2019-10-21.
//

#include "VKSwapChain.h"
#include "VKQueue.h"

NS_RHI_BEGIN

VKSwapChain::VKSwapChain(VKDevice* device, VkFormat targetSurfaceFormat)
    : device_(device)
    , format_(GetTextureFormat(targetSurfaceFormat))
{
    CreateVulkanSwapChain(targetSurfaceFormat);
    Init();
}

VKSwapChain::~VKSwapChain()
{
    Dispose();
}

void VKSwapChain::CreateVulkanSwapChain(VkFormat targetSurfaceFormat)
{
    auto queueFamilyIndices_ = device_->GetQueueFamilyIndices();
    auto vkSurface_ = device_->GetVulkanSurface();
    
    std::uint32_t queueFamilyIndices[] = {
        (std::uint32_t)queueFamilyIndices_.graphicsFamily,
        (std::uint32_t)queueFamilyIndices_.presentFamily
    };
    
    SwapChainSupportDetails swapChainSupport = device_->QuerySwapChainSupport(vkSurface_);
    VkSurfaceFormatKHR surfaceFormat = device_->ChooseVulkanSurfaceFormat(swapChainSupport.formats, targetSurfaceFormat);
    vkSwapchainImageFormat_ = surfaceFormat.format;
    extent_ = device_->ChooseSwapExtent(swapChainSupport.capabilities);
    
    uint32_t minImageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        minImageCount > swapChainSupport.capabilities.maxImageCount)
    {
        minImageCount = swapChainSupport.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = vkSurface_,
        .minImageCount = minImageCount,
        .imageFormat = vkSwapchainImageFormat_,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = {extent_.width, extent_.height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
    };
    
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
    CALL_VK(vkCreateSwapchainKHR(device_->GetDevice(), &createInfo, nullptr, &vkSwapChain_));
}

void VKSwapChain::Init()
{
    auto vkDevice = device_->GetDevice();
    
    std::vector<VkImage> swapChainImages;
    uint32_t imageCount = 0;
    
    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain_, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain_, &imageCount,
                            swapChainImages.data());
    
    Extent3D extent3D = extent_;
    
    // setup swapChainImageViews_
    swapChainImageViews_.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = vkSwapchainImageFormat_;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        auto textureImage = new VKTextureView(device_, createInfo, extent_);
        RHI_SAFE_RETAIN(textureImage);
        swapChainImageViews_[i] = textureImage;
    }
    
    // Create a semaphore to sync swapchain
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    CALL_VK(vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &vkImageAvailableSemaphore_));
}

void VKSwapChain::Dispose()
{
    for (auto textureView : swapChainImageViews_)
    {
        RHI_SAFE_RELEASE(textureView);
    }
    swapChainImageViews_.clear();
    vkDestroySemaphore(device_->GetDevice(), vkImageAvailableSemaphore_, nullptr);
}

void VKSwapChain::RecreateSwapChain()
{
    Dispose();
    Init();
}

TextureView* VKSwapChain::GetCurrentTexture()
{
    // TODO realxie recreate swapchain
    
    CALL_VK(vkAcquireNextImageKHR(device_->GetDevice(), vkSwapChain_,
                                  std::numeric_limits<uint64_t>::max(),
                                  vkImageAvailableSemaphore_, VK_NULL_HANDLE, &imageIndex_));
    
    device_->AddWaitingSemaphore(vkImageAvailableSemaphore_);
    return swapChainImageViews_[imageIndex_];
    
    //    const VkResult code = vkAcquireNextImageKHR(device_->GetDevice(),
    //            device_->GetVkSwapChain(),
    //            std::numeric_limits<uint64_t>::max(),
    //            vkImageAvailableSemaphore_,
    //            VK_NULL_HANDLE,
    //            &imageIndex_);
    //
    //    if (code == VK_SUCCESS)
    //    {
    //        device_->AddWaitingSemaphore(vkImageAvailableSemaphore_);
    //        return swapChainImageViews_[imageIndex_];
    //    }
    //
    //    if (code == VK_ERROR_SURFACE_LOST_KHR)
    //    {
    //        RecreateSwapChain();
    //        CALL_VK(vkAcquireNextImageKHR(device_->GetDevice(), device_->GetVkSwapChain(), std::numeric_limits<uint64_t>::max(),
    //                                      vkImageAvailableSemaphore_, VK_NULL_HANDLE, &imageIndex_));
    //
    //        device_->AddWaitingSemaphore(vkImageAvailableSemaphore_);
    //        return swapChainImageViews_[imageIndex_];
    //    }
    //    else
    //    {
    //        RHI_ASSERT(false);
    //    }
}

void VKSwapChain::Present(const Queue* queue)
{
//    VkSwapchainKHR vkSwapchain = device_->GetVkSwapChain();
    VkResult result;
    VkPresentInfoKHR presentInfo;
    {
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &vkSwapChain_;
        presentInfo.pImageIndices = &imageIndex_;
        presentInfo.waitSemaphoreCount = 0;
        presentInfo.pWaitSemaphores = nullptr;
        presentInfo.pResults = &result;
    }
    
    VkQueue vkQueue = RHI_CAST(const VKQueue*, queue)->GetNative();
    if (auto code = vkQueuePresentKHR(vkQueue, &presentInfo); code == VK_ERROR_OUT_OF_DATE_KHR ||
                                                              code == VK_SUBOPTIMAL_KHR)
    {
        // TODO: recreate swapchain
    }
    else if (code != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
    
    vkQueueWaitIdle(vkQueue);
}

NS_RHI_END
