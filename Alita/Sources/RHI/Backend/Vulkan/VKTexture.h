//
// Created by realxie on 2019-10-07.
//

#ifndef ALITA_VKTEXTURE_H
#define ALITA_VKTEXTURE_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKTexture : public Texture
{
protected:
    VKTexture() = default;
    
    virtual ~VKTexture();
    
    bool Init(VKDevice* device, const TextureDescriptor &descriptor);

public:
    VkImage GetNative() const
    { return vkImage_; }
    
    VkFormat GetNativeFormat() const
    { return vkFormat_; }
    
    Extent3D GetTextureSize() const
    { return textureSize_; }
    
    virtual TextureFormat GetFormat() const override
    { return textureFormat_; };
    
    virtual TextureView* CreateView() override;

private:
    void SetImageLayout(const VKDevice* device);

private:
    VKDevice* device_ = nullptr;
    VkDevice vkDevice_ = nullptr;
    VkImage vkImage_ = 0L;
    VkDeviceMemory vkDeviceMemory_ = 0L;
    VkFormat vkFormat_ = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
    TextureFormat textureFormat_;
    
    Extent3D textureSize_;
    
    friend class VKDevice;
};

NS_RHI_END


#endif //ALITA_VKTEXTURE_H
