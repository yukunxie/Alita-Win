//
// Created by realxie on 2019-10-07.
//

#ifndef RHI_SAMPLER_H
#define RHI_SAMPLER_H

#include "Macros.h"
#include "BindingResource.h"


NS_RHI_BEGIN

class Sampler;

class SamplerBinding;

class Sampler : public RHIObjectBase
{
public:
    Sampler(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::Sampler)
    {}

protected:
    virtual ~Sampler() = default;
};

class SamplerBinding final : public BindingResource
{
public:
    SamplerBinding(Device* device)
        : BindingResource(device, BindingResourceType::Sampler)
    {
    }
    
    virtual ~SamplerBinding() = default;
    
    bool Init(Sampler* sampler)
    {
        sampler_ = sampler;
        return true;
    }
    
    Sampler* GetSampler()
    { return sampler_.Get(); }
    
    virtual void Dispose() override
    {}

protected:
    RHIObjectWrapper<Sampler> sampler_ = nullptr;
};

NS_RHI_END

#endif //RHI_SAMPLER_H
