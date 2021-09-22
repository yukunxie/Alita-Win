//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_TEXTUREVIEW_H
#define RHI_TEXTUREVIEW_H

#include "Macros.h"
#include "BindingResource.h"

NS_RHI_BEGIN

class TextureView : public RHIObjectBase
{
public:
    TextureView(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::TextureView)
    {
    }
    
    bool IsSwapchainImage()
    { return isSwapchainImage_; }
    
    void MarkSwapchainImage()
    { isSwapchainImage_ = true; }

protected:
    virtual ~TextureView() = default;

public:
    virtual TextureFormat GetFormat() const = 0;

protected:
    bool isSwapchainImage_ = false;
};

class TextureViewBinding final : public BindingResource
{
public:
    TextureViewBinding(Device* device)
        : BindingResource(device, BindingResourceType::TextureView)
    {
    }
    
    virtual ~TextureViewBinding() = default;
    
    bool Init(TextureView* textureView)
    {
        textureView_ = textureView;
        return true;
    }
    
    TextureView* GetTextureView()
    {
        return textureView_.Get();
    }
    
    virtual void Dispose() override
    {}

protected:
    RHIObjectWrapper<TextureView> textureView_;
};

NS_RHI_END

#endif //RHI_TEXTUREVIEW_H
