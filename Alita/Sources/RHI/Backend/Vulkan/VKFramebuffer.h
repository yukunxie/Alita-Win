//
// Created by realxie on 2019-12-25.
//

#ifndef RHI_VKFRAMEBUFFER_H
#define RHI_VKFRAMEBUFFER_H

#include "VKDevice.h"
#include <vector>
#include <vulkan/vulkan.h>

NS_RHI_BEGIN

class VKFramebuffer final : public Framebuffer
{
protected:
    VKFramebuffer(VKDevice* device);
    
    ~VKFramebuffer();

public:
    
    bool Init(const FramebufferCacheQuery &query);
    
    VkFramebuffer GetNative()
    {
        Prepare();
        return vkFramebuffer_;
    }
    
    const Extent2D &GetExtent2D()
    { return extent2D_; };
    
    virtual void Dispose() override;
    
    void Prepare();
    
    void Invalidate();
    
    std::uint32_t GetColorAttachmentCount() const
    {
        return colorAttachmentCount_;
    }
    
    const std::array<const VKTextureView*, kMaxColorAttachments>& GetColorAttachments() const
    {
        return colorAttachments_;
    }
    
    bool HasSwapChainImages()
    { return hasSwapChainImages_;}

private:
    VkFramebuffer vkFramebuffer_ = VK_NULL_HANDLE;
    VKRenderPass* renderPass_ = nullptr;
    Extent2D extent2D_;
    
    std::uint32_t colorAttachmentCount_ = 0;
    std::array<const VKTextureView*, kMaxColorAttachments> colorAttachments_;
    
    bool hasSwapChainImages_ = false;
    
    friend class VKDevice;
};

NS_RHI_END


#endif //RHI_VKFRAMEBUFFER_H
