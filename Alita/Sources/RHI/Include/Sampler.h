//
// Created by realxie on 2019-10-07.
//

#ifndef ALITA_SAMPLER_H
#define ALITA_SAMPLER_H

#include "Macros.h"
#include "BindingResource.h"


NS_RHI_BEGIN

class Sampler;

class Sampler : public BindingResource
{
public:
    Sampler() : BindingResource(BindingResourceType::SAMPLER)
    {}

protected:
    virtual ~Sampler()
    {
    }
};

NS_RHI_END

#endif //ALITA_SAMPLER_H
