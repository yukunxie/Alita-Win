//
// Created by realxie on 2019-10-21.
//

#ifndef RHI_VKSWAPCHAIN_H
#define RHI_VKSWAPCHAIN_H

#include "VKDevice.h"
#include "VKTextureView.h"

NS_GFX_BEGIN

struct FrameResource
{
    VKSwapChain* swapChain = nullptr;
    std::uint32_t imageIndex = -1;
    VkSemaphore semaPresentable = VK_NULL_HANDLE;
    VkSemaphore semaImageAvaliable = VK_NULL_HANDLE;
    VkFence     fenceToSyncSubmission = VK_NULL_HANDLE;
    
    FrameResource& operator= (const FrameResource& other)
    {
        swapChain = other.swapChain;
        imageIndex = other.imageIndex;
        semaPresentable = other.semaPresentable;
        semaImageAvaliable = other.semaImageAvaliable;
        fenceToSyncSubmission = other.fenceToSyncSubmission;
        return *this;
    }
};

struct RenderingFrameInfo
{
    const FrameResource* frameResource = nullptr;
    VkCommandBuffer      vkCommandBuffer = VK_NULL_HANDLE;
    std::vector<VKCommandBuffer*> commandBuffers;
    
    bool IsValid()
    {
        return !!frameResource;
    }
};

class VKSwapChain final : public SwapChain
{
protected:
    VKSwapChain(VKDevice* device);
    
    virtual ~VKSwapChain();

public:
    
    bool Init(const SwapChainDescriptor& descriptor);
    
    virtual Texture* GetCurrentTexture() override;
    
    void Present(Queue* queue, VkSemaphore waitingSemaphore) = delete;
    
    virtual Extent2D GetExtent() override
    { return extent_; };
    
    virtual TextureFormat GetFormat() override
    { return format_; }
    
    virtual void Dispose() override;

    virtual void RecreateSwapChain() override;
    
    void AcquireNextImage();
    
    VkSwapchainKHR GetNative()
    { return vkSwapChain_; }
    
    void NotifyPresentDone(bool hasDrawCommands);
    
    std::uint32_t GetImageIndex()
    {
        return imageIndex_;
    }
    
    std::uint32_t GetImageCount()
    {
        return imageCount_;
    }
    
    FrameResource* GetFrameResource()
    {
        return frameResources_ + imageIndex_;
    }
    
    void SetRenderingFrameInfo(FrameResource* frameResource, VkCommandBuffer vkCommandBuffer, ssize_t count,  VKCommandBuffer **commandBuffers)
    {
        GFX_ASSERT(renderingFrameInfo_.commandBuffers.empty());
        renderingFrameInfo_.frameResource = frameResource;
        renderingFrameInfo_.vkCommandBuffer = vkCommandBuffer;
        for (int i = 0; i < count; ++i)
        {
            renderingFrameInfo_.commandBuffers.push_back(commandBuffers[i]);
        }
    }
    
    RenderingFrameInfo* GetRenderingFrameInfo()
    {
        return &renderingFrameInfo_;
    }
    
    bool AcquireNextImageKHR(VkSemaphore semaphore, std::uint32_t* pImageIndex);
    
    void Present(std::uint32_t imageIndex, VkSemaphore semaWaiting, bool hasDrawCommands);
    
protected:
    void Init();
    
    void CreateVulkanSwapChain();
    
    bool RecreateSwapChainInternal();

private:
    VkSwapchainKHR vkSwapChain_ = VK_NULL_HANDLE;
    VkImageUsageFlags vkUsages_ = VK_NULL_HANDLE;
    FrameResource frameResources_[3];
    RenderingFrameInfo renderingFrameInfo_;
    VkFormat vkSwapchainImageFormat_;
    std::vector<VKTexture*> swapchainTextures_;
    std::uint32_t imageIndex_ = 0;
    std::uint32_t imageCount_ = 3;
    TextureFormat format_;
    Extent2D extent_;
    TextureUsageFlags textureUsage_;
    
    std::unique_ptr<SemaphoreCXX> pSemaphoreCxx_;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKSWAPCHAIN_H
