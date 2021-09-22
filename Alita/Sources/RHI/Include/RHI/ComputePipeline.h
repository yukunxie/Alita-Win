//
// Created by realxie on 2020-01-19.
//

#ifndef RHI_COMPUTEPIPELINE_H
#define RHI_COMPUTEPIPELINE_H

#include "Macros.h"
#include "Flags.h"
#include "BindGroupLayout.h"
#include "PipelineLayout.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class ComputePipeline : public RHIObjectBase
{
public:
    ComputePipeline(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::ComputePipeline)
    {}

protected:
    virtual ~ComputePipeline() = default;
};

NS_RHI_END

#endif //RHI_COMPUTEPIPELINE_H
