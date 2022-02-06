//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_PIPELINELAYOUT_H
#define RHI_PIPELINELAYOUT_H

#include "Macros.h"
#include "GFXBase.h"

NS_GFX_BEGIN

class PipelineLayout : public GfxBase
{
public:
    PipelineLayout(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::PipelineLayout)
    {}

    virtual ~PipelineLayout() = default;
};

NS_GFX_END

#endif //RHI_PIPELINELAYOUT_H
