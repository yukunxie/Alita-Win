//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_BINDINGRESOURCE_H
#define RHI_BINDINGRESOURCE_H

#include "Macros.h"
#include "GFXBase.h"

NS_GFX_BEGIN

class Sampler;

class TextureView;

enum class BindingResourceType
{
    Buffer,
    Sampler,
    TextureView
};

class BindingResource : public GfxBase
{
public:
    BindingResource(DevicePtr device, BindingResourceType bindingResourceType)
        : GfxBase(device, RHIObjectType::BindingResource)
    {
        bindingResourceType_ = bindingResourceType;
    }

    BindingResourceType GetResourceType() const
    {
        return bindingResourceType_;
    }

    virtual ~BindingResource() = default;
    
protected:
    BindingResourceType bindingResourceType_;
};



NS_GFX_END

#endif //RHI_BINDINGRESOURCE_H
