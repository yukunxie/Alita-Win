//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_VKTEXTUREVIEW_H
#define RHI_VKTEXTUREVIEW_H

#include "VKDevice.h"
#include "VKTexture.h"

NS_RHI_BEGIN

class VKTextureViewManager;

class VKTextureView final : public TextureView
{
protected:
    VKTextureView(VKDevice* device);
    
    virtual ~VKTextureView();
    
    void Recreate();

public:
    
    bool Init(VKTexture* vkTexture, const TextureViewDescriptor &descriptor);
    
    VkImageView GetNative() const
    { return vkImageView_; }
    
    Extent3D GetTextureSize() const
    { return textureSize_; }
    
    std::uint32_t GetSampleCount() const
    {
        return texture_ ? texture_->GetSampleCount() : 1;
    }
    
    virtual TextureFormat GetFormat() const override
    { return textureFormat_; }
    
    virtual void Dispose() override;
    
    VkImage GetImage()
    {
        return texture_ ? texture_->GetNative() : VK_NULL_HANDLE;
    }
    
    VKTexture* GetTexture()
    { return texture_.Get(); }

private:
    RHIObjectWrapper<VKTexture> texture_ = nullptr;
    
    VkImageView vkImageView_ = VK_NULL_HANDLE;
    
    Extent3D textureSize_;
    TextureFormat textureFormat_;
    
    TextureViewDescriptor textureViewDescriptor_;
    
    friend class VKDevice;
    
    friend class VKTextureViewManager;
    
    friend class VKTexture;
};

NS_RHI_END


#endif //RHI_VKTEXTUREVIEW_H
