//
// Created by realxie on 2020-01-19.
//

#ifndef RHI_COMPUTEPASSENCODER_H
#define RHI_COMPUTEPASSENCODER_H


#include "Macros.h"
#include "GFXBase.h"
#include "ComputePipeline.h"
#include "Buffer.h"

NS_GFX_BEGIN

class VKCommandBuffer;

class ComputePassEncoder : public GfxBase
{
public:
    ComputePassEncoder(const DevicePtr& GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::ComputePassEncoder)
    {}

    virtual ~ComputePassEncoder() = default;

public:
    virtual void SetPipeline(const ComputePipelinePtr& pipeline) = 0;
    
    virtual void Dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1) = 0;
    
    virtual void DispatchIndirect(const BufferPtr& indirectBuffer, BufferSize indirectOffset) = 0;
    
    virtual void EndPass() = 0;
    
    virtual void SetBindGroup(std::uint32_t index, const BindGroupPtr& bindGroup, std::uint32_t count,
                              std::uint32_t* dynamicOffsets) = 0;
    
    virtual void PushDebugGroup(const std::string &groupLabel) = 0;
    
    virtual void PopDebugGroup() = 0;
    
    virtual void InsertDebugMarker(const std::string &markerLabel) = 0;
};

NS_GFX_END


#endif //RHI_COMPUTEPASSENCODER_H
