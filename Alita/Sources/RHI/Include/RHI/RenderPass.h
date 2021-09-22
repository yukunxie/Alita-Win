//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_RENDERPASS_H
#define RHI_RENDERPASS_H

#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class RenderPass : public RHIObjectBase
{
public:
    RenderPass(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::RenderPass)
    {}

protected:
    virtual ~RenderPass() = default;
};

NS_RHI_END

#endif //RHI_RENDERPASS_H
