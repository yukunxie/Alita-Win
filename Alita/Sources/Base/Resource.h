//
// Created by realxie on 2019-10-29.
//


#pragma once

#include "Macros.h"
#include "ObjectBase.h"
#include "Types/Types.h"

NS_RX_BEGIN

enum class EResourceType
{
    Texture,
    Sampler,
    RenderTarget,
    Buffer,
};

class Resource : public ObjectBase
{
public:
    Resource(EResourceType type)
        : ResourceType_(type)
    {
    }

    virtual ~Resource() {};

    EResourceType GetResourceType() const
    {
        return ResourceType_;
    }

protected:
    const EResourceType ResourceType_;
};

NS_RX_END

