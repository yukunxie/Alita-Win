//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_VKTEXTUREVIEW_H
#define RHI_VKTEXTUREVIEW_H

#include "VKDevice.h"
#include "VKTexture.h"

NS_GFX_BEGIN

class TextureViewManager;

class VKTextureView final : public TextureView
{
protected:
    VKTextureView(DevicePtr device);
    
    virtual void Recreate() override;

public:
    virtual ~VKTextureView();

public:
    
    bool Init(const TexturePtr& vkTexture, const TextureViewDescriptor& descriptor);

    VkImageView GetNative() const
    {
        return vkImageView_;
    }

    Extent3D GetTextureSize() const
    {
        return texture_->GetTextureSize();
    }

    std::uint32_t GetSampleCount() const
    {
        return texture_ ? texture_->GetSampleCount() : 1;
    }

    virtual TextureFormat GetFormat() const override
    {
        return textureFormat_;
    }

    virtual void Dispose() override;

    VkImage GetImage()
    {
        return texture_ ? GFX_CAST(VKTexture*, texture_)->GetNative() : VK_NULL_HANDLE;
    }

    virtual const TexturePtr& GetTexture() const override
    {
        return texture_;
    }

private:
    TexturePtr texture_ = nullptr;
    
    VkImageView vkImageView_ = VK_NULL_HANDLE;
    
    //Extent3D textureSize_;
    TextureFormat textureFormat_;
    
    TextureViewDescriptor textureViewDescriptor_;
    
    friend class VKDevice;
    
    friend class TextureViewManager;
    
    friend class VKTexture;
};

NS_GFX_END


#endif //RHI_VKTEXTUREVIEW_H
