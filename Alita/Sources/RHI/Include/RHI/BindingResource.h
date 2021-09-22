//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_BINDINGRESOURCE_H
#define RHI_BINDINGRESOURCE_H

#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class Sampler;

class TextureView;

enum class BindingResourceType
{
    Buffer,
    Sampler,
    TextureView
};

class BindingResource : public RHIObjectBase
{
public:
    BindingResource(Device* device, BindingResourceType bindingResourceType)
        : RHIObjectBase(device, RHIObjectType::BindingResource)
    {
        bindingResourceType_ = bindingResourceType;
    }
    
    BindingResourceType GetResourceType() const
    { return bindingResourceType_;}

protected:
    virtual ~BindingResource() = default;
    
    BindingResourceType bindingResourceType_;
};



NS_RHI_END

#endif //RHI_BINDINGRESOURCE_H
