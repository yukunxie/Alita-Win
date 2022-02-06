//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_TEXTUREVIEW_H
#define RHI_TEXTUREVIEW_H

#include "Macros.h"
#include "BindingResource.h"

NS_GFX_BEGIN

class TextureView : public GfxBase
{
public:
    TextureView(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::TextureView)
    {
    }
    
    bool IsSwapchainImage() const
    { return isSwapchainImage_; }
    
    void MarkSwapchainImage()
    { isSwapchainImage_ = true; }

    virtual const TexturePtr& GetTexture() const { return {}; }

    virtual void Recreate() = 0;

    virtual ~TextureView() = default;

public:
    virtual TextureFormat GetFormat() const = 0;

protected:
    bool isSwapchainImage_ = false;
};

class TextureViewBinding final : public BindingResource
{
public:
    TextureViewBinding(const DevicePtr& device)
        : BindingResource(device, BindingResourceType::TextureView)
    {
    }
    
    virtual ~TextureViewBinding() = default;
    
    bool Init(const TextureViewPtr& textureView)
    {
        textureView_ = textureView;
        return true;
    }
    
    const TextureViewPtr& GetTextureView()
    {
        return textureView_;
    }
    
    virtual void Dispose() override
    {}

protected:
    TextureViewPtr textureView_;
};

NS_GFX_END

#endif //RHI_TEXTUREVIEW_H
