//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_RENDERPIPLEINE_H
#define RHI_RENDERPIPLEINE_H

#include "Macros.h"
#include "Flags.h"
#include "BindGroupLayout.h"
#include "PipelineLayout.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class RenderPipeline : public RHIObjectBase
{
public:
    RenderPipeline(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::RenderPipeline)
    {}
    
    FORCE_INLINE IndexFormat GetIndexFormat()
    {return indexFormat_;}
    
    bool Init(const RenderPipelineDescriptor &descriptor)
    {
        indexFormat_ = descriptor.vertexState.indexFormat;
        return true;
    }

protected:
    virtual ~RenderPipeline() = default;

protected:
    IndexFormat indexFormat_ = IndexFormat::UINT16;
};

NS_RHI_END

#endif //RHI_RENDERPIPLEINE_H
