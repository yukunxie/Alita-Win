//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_RENDERPIPLEINE_H
#define RHI_RENDERPIPLEINE_H

#include "Macros.h"
#include "Flags.h"
#include "BindGroupLayout.h"
#include "PipelineLayout.h"
#include "GFXBase.h"

NS_GFX_BEGIN

class RenderPipeline : public GfxBase
{
public:
    RenderPipeline(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::RenderPipeline)
    {}
    
    FORCE_INLINE IndexFormat GetIndexFormat()
    {return indexFormat_;}
    
    bool Init(const RenderPipelineDescriptor &descriptor)
    {
        indexFormat_ = descriptor.vertexState.indexFormat;
        return true;
    }

    virtual ~RenderPipeline() = default;

protected:
    IndexFormat indexFormat_ = IndexFormat::UINT16;
};

NS_GFX_END

#endif //RHI_RENDERPIPLEINE_H
