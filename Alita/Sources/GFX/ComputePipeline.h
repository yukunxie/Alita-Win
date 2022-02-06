//
// Created by realxie on 2020-01-19.
//

#ifndef RHI_COMPUTEPIPELINE_H
#define RHI_COMPUTEPIPELINE_H

#include "Macros.h"
#include "Flags.h"
#include "BindGroupLayout.h"
#include "PipelineLayout.h"
#include "GFXBase.h"

NS_GFX_BEGIN

class ComputePipeline : public GfxBase
{
public:
    ComputePipeline(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::ComputePipeline)
    {}

    virtual ~ComputePipeline() = default;
};

NS_GFX_END

#endif //RHI_COMPUTEPIPELINE_H
