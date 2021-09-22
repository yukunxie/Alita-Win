//
// Created by realxie on 2019-10-11.
//

#ifndef RHI_RENDERTARGET_H
#define RHI_RENDERTARGET_H


#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class RenderTarget : public RHIObjectBase
{
public:
    RenderTarget()
        : RHIObjectBase(RHIObjectType::RenderTarget)
    {}

protected:
    virtual ~RenderTarget() = default;
};

NS_RHI_END

#endif //RHI_RENDERTARGET_H
