//
// Created by realxie on 2019-10-10.
//

#ifndef ALITA_BINDINGRESOURCE_H
#define ALITA_BINDINGRESOURCE_H

#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

enum class BindingResourceType
{
    UNDEFINED,
    BUFFER,
    SAMPLER,
    TEXTURE_VIEW,
    COMBINED_SAMPLER_TEXTUREVIEW,
};

class Sampler;

class TextureView;

class BindingResource : public RHIObjectBase
{
public:
    BindingResource(BindingResourceType bindingResourceType)
        : bindingResourceType_(bindingResourceType)
    {
    }
    
    virtual ~BindingResource()
    {
    }
    
    BindingResourceType GetResourceType() const
    { return bindingResourceType_; }

protected:
    BindingResourceType bindingResourceType_;
};

struct CombinedSamplerImageViewBinding : public BindingResource
{
    CombinedSamplerImageViewBinding()
        : BindingResource(BindingResourceType::COMBINED_SAMPLER_TEXTUREVIEW)
    {}
    
    CombinedSamplerImageViewBinding(const Sampler* sampler, const TextureView* imageView)
        : BindingResource(BindingResourceType::COMBINED_SAMPLER_TEXTUREVIEW), sampler(sampler),
          imageView(imageView)
    {}
    
    const Sampler* sampler = nullptr;
    const TextureView* imageView = nullptr;
};

NS_RHI_END

#endif //ALITA_BINDINGRESOURCE_H
