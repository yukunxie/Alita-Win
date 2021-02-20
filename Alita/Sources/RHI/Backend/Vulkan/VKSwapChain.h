//
// Created by realxie on 2019-10-21.
//

#ifndef ALITA_VKSWAPCHAIN_H
#define ALITA_VKSWAPCHAIN_H

#include "VKDevice.h"
#include "VKTextureView.h"

NS_RHI_BEGIN

class VKSwapChain : public SwapChain
{
public:
    VKSwapChain(VKDevice* device, VkFormat targetSurfaceFormat);
    
    virtual ~VKSwapChain();

public:
    virtual TextureView* GetCurrentTexture() override;
    
    virtual void Present(const Queue* queue) override;
    
    virtual Extent2D GetExtent() override
    {return extent_;};
    
    virtual TextureFormat GetFormat() override
    {return format_;}

protected:
    void Init();
    
    void Dispose();
    
    void RecreateSwapChain();
    
    void CreateVulkanSwapChain(VkFormat targetSurfaceFormat);

private:
    VKDevice*       device_ = nullptr;
    VkSwapchainKHR  vkSwapChain_ = 0;
    VkSemaphore     vkImageAvailableSemaphore_ = 0L;
    std::vector<VKTextureView*> swapChainImageViews_;
    std::uint32_t   imageIndex_ = 0;
    VkFormat        vkSwapchainImageFormat_;
    TextureFormat   format_;
    Extent2D        extent_;
//    VkExtent2D  vkSwapchainExtent_;
};

NS_RHI_END


#endif //ALITA_VKSWAPCHAIN_H
