//
// Created by realxie on 2019-10-07.
//

#ifndef RHI_SAMPLER_H
#define RHI_SAMPLER_H

#include "Macros.h"
#include "BindingResource.h"


NS_GFX_BEGIN

class Sampler;

class SamplerBinding;

class Sampler : public GfxBase
{
public:
    Sampler(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::Sampler)
    {}

protected:
    virtual ~Sampler() = default;
};

class SamplerBinding final : public BindingResource
{
public:
    SamplerBinding(const DevicePtr& device)
        : BindingResource(device, BindingResourceType::Sampler)
    {
    }
    
    virtual ~SamplerBinding() = default;
    
    bool Init(const SamplerPtr& sampler)
    {
        sampler_ = sampler;
        return true;
    }
    
    const SamplerPtr& GetSampler()
    { return sampler_; }
    
    virtual void Dispose() override
    {}

protected:
    SamplerPtr sampler_ = nullptr;
};

NS_GFX_END

#endif //RHI_SAMPLER_H
