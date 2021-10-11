//
// Created by realxie on 2019-10-11.
//

#ifndef RHI_RENDERTARGET_H
#define RHI_RENDERTARGET_H


#include "Macros.h"
#include "GFXBase.h"

NS_GFX_BEGIN

class RenderTarget : public GfxBase
{
public:
    RenderTarget()
        : GfxBase(RHIObjectType::RenderTarget)
    {}

protected:
    virtual ~RenderTarget() = default;
};

NS_GFX_END

#endif //RHI_RENDERTARGET_H
