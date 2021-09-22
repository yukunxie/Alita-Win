//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_PIPELINELAYOUT_H
#define RHI_PIPELINELAYOUT_H

#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class PipelineLayout : public RHIObjectBase
{
public:
    PipelineLayout(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::PipelineLayout)
    {}

protected:
    virtual ~PipelineLayout() = default;
};

NS_RHI_END

#endif //RHI_PIPELINELAYOUT_H
