//
// Created by realxie on 2019-10-10.
//

#ifndef ALITA_VKTEXTUREVIEW_H
#define ALITA_VKTEXTUREVIEW_H

#include "VKDevice.h"
#include "VKTexture.h"

NS_RHI_BEGIN

class VKTextureView final : public TextureView
{
public:
    VKTextureView(VKDevice* device, VKTexture* vkTexture);
    
    VKTextureView(VKDevice* device, const VkImageViewCreateInfo &imageViewCreateInfo,
                  const Extent3D &textureSize);
    
    virtual ~VKTextureView();
    
    VkImageView GetNative() const
    { return vkImageView_; }

    VkImage GetVkImage()
    {
        return vkImage_;
    }

    void SetVkImage(VkImage img)
    {
        vkImage_ = img;
    }
    
    Extent3D GetTextureSize() const
    { return textureSize_; }
    
    virtual TextureFormat GetFormat() const override
    { return textureFormat_; }

private:
    VkDevice vkDevice_ = nullptr;
    VkImageView vkImageView_ = 0L;
    VkImage vkImage_ = nullptr;

    VKTexture* texture_ = nullptr;
    
    Extent3D textureSize_;
    TextureFormat textureFormat_;
    
    
};

NS_RHI_END


#endif //ALITA_VKTEXTUREVIEW_H
