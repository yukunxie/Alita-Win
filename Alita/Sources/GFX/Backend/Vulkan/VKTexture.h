//
// Created by realxie on 2019-10-07.
//

#ifndef RHI_VKTEXTURE_H
#define RHI_VKTEXTURE_H

#include "VKDevice.h"

NS_GFX_BEGIN

class VKTextureView;

class VKSwapChain;

class VKCommandBuffer;

VkImageLayout GetVulkanImageLayout(TextureUsageFlags usage, TextureFormat format);

class VKTexture final : public Texture
{
protected:
    VKTexture(DevicePtr device);
    
public:
    virtual ~VKTexture();

protected:
    void DisposeNativeHandle();
    
    static void SetVkImageHandleDirectly(const TexturePtr& texture, VkImage vkImage);

public:
    
    bool Init(const TextureDescriptor &descriptor);
    
    bool Init(VkImage vkImage, const TextureDescriptor &descriptor);
    
    VkImage GetNative() const
    {
        return vkImage_;
    }

    VkFormat GetNativeFormat() const
    {
        return vkFormat_;
    }
    
    virtual void Dispose() override;
    
    static VkImageAspectFlags GetVkImageAspectFlags(VkFormat format)
    {
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        if (VK_FORMAT_D24_UNORM_S8_UINT == format)
        {
            aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        return aspectMask;
    }
    
    void TransToCopySrcImageLayout(VKCommandBuffer* commandBuffer);
    
    void TransToCopyDstImageLayout(VKCommandBuffer* commandBuffer);
    
    void TransToOutputAttachmentImageLayout(VKCommandBuffer* commandBuffer);
    
    void TransToSampledImageLayout(VKCommandBuffer* commandBuffer);

    TextureUsageFlags GetTextureMemoryLayout() const
    {
        return currentTexUsageForMemLayout_;
    }

    void SetTextureMemoryLayout(TextureUsageFlags flags)
    {
        currentTexUsageForMemLayout_ = flags;
    }

private:
    void TransToTargetImageLayout_(VKCommandBuffer* commandBuffer, TextureUsageFlags targetTextureUsage);

private:
    VkDevice vkDevice_ = VK_NULL_HANDLE;
    VkImage vkImage_ = VK_NULL_HANDLE;
    
    VkFormat vkFormat_ = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
    
    TextureUsageFlags  currentTexUsageForMemLayout_ = TextureUsage::UNDEFINED;

#if USE_VULKAN_MEMORY_ALLCATOR
    VmaAllocation vmaAllocation_ = nullptr;
    VmaAllocationInfo vmaAllocationInfo_ = {};
#else
    VkDeviceMemory vkDeviceMemory_ = VK_NULL_HANDLE;
#endif
    
    bool isVkImageWrapper_ = false;

    bool isSwapChainTexture_ = false;
    
    friend class VKDevice;
    
    friend class VKSwapChain;

public:
    inline void SetIsSwapChainTexture(bool flag) { isSwapChainTexture_ = flag; }
    inline bool GetIsSwapChainTexture() const { return isSwapChainTexture_; }
};

NS_GFX_END


#endif //RHI_VKTEXTURE_H
