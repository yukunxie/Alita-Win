//
// Created by realxie on 2019-10-10.
//

#include "VKTextureView.h"
#include "VKTypes.h"

NS_GFX_BEGIN

VKTextureView::VKTextureView(DevicePtr device)
    : TextureView(device)
{
}

void VKTextureView::Recreate()
{
    if (vkImageView_)
    {
        vkDestroyImageView(VKDEVICE()->GetNative(), vkImageView_,
                             nullptr);
        vkImageView_ = VK_NULL_HANDLE;
    }
    
    Init(texture_, textureViewDescriptor_);
}

void VKTextureView::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    if (vkImageView_)
    {
        vkDestroyImageView(VKDEVICE()->GetNative(), vkImageView_,
                             nullptr);
        vkImageView_ = VK_NULL_HANDLE;
    }
    texture_.reset();
    
    GFX_DISPOSE_END();
}

VKTextureView::~VKTextureView()
{
    Dispose();
}

bool VKTextureView::Init(const TexturePtr& vkTexture, const TextureViewDescriptor &descriptor)
{
    if (vkTexture->IsSwapchainImage())
    {
        MarkSwapchainImage();
    }
    
    texture_ = vkTexture;
    textureViewDescriptor_ = descriptor;
    
    //textureSize_ = texture_->GetTextureSize();
    textureFormat_ = texture_->GetFormat();
    
    VkImageViewCreateInfo viewInfo = {};
    {
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.flags = 0;
        viewInfo.image = GFX_CAST(VKTexture*, texture_)->GetNative();
        viewInfo.viewType = ToVulkanType(textureViewDescriptor_.dimension);
        viewInfo.format = GFX_CAST(VKTexture*, texture_)->GetNativeFormat();
        
        viewInfo.components = {};
        {
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        }
        
        viewInfo.subresourceRange = {};
        {
            auto aspectMask = ToVulkanType(textureViewDescriptor_.aspect,  GFX_CAST(VKTexture*, texture_)->GetNativeFormat());
            viewInfo.subresourceRange.aspectMask = aspectMask;
            
            std::uint32_t levelCount = textureViewDescriptor_.mipLevelCount;
            if (0 == levelCount)
            {
                levelCount = texture_->GetMipLevelCount() - textureViewDescriptor_.baseMipLevel;
            }
            viewInfo.subresourceRange.baseMipLevel = textureViewDescriptor_.baseMipLevel;
            viewInfo.subresourceRange.levelCount = levelCount;
            
            std::uint32_t layerCount = textureViewDescriptor_.arrayLayerCount;
            if (0 == layerCount)
            {
                layerCount = texture_->GetArrayLayerCount() - textureViewDescriptor_.baseArrayLayer;
            }
            viewInfo.subresourceRange.baseArrayLayer = textureViewDescriptor_.baseArrayLayer;
            viewInfo.subresourceRange.layerCount = layerCount;
        }
    }
    CALL_VK(vkCreateImageView(VKDEVICE()->GetNative(), &viewInfo, nullptr, &vkImageView_));
    
    return VK_NULL_HANDLE != vkImageView_;
}

NS_GFX_END
