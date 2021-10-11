//
// Created by realxie on 2020-01-19.
//

#ifndef RHI_VKCOMPUTEPASSENCODER_H
#define RHI_VKCOMPUTEPASSENCODER_H


#include "VKDevice.h"
#include "VKPipelineLayout.h"

NS_GFX_BEGIN

class VKComputePassEncoder final : public ComputePassEncoder
{
protected:
    VKComputePassEncoder(VKDevice* device);
    
    ~VKComputePassEncoder();

public:
    
    bool Init(VKCommandBuffer* commandBuffer, const ComputePassDescriptor &descriptor);
    
    virtual void SetPipeline(ComputePipeline* pipeline) override;
    
    virtual void Dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1) override;
    
    virtual void DispatchIndirect(Buffer* indirectBuffer, BufferSize indirectOffset) override;
    
    virtual void EndPass() override;
    
    virtual void Dispose() override;
    
    virtual void SetBindGroup(std::uint32_t index, BindGroup* bindGroup, std::uint32_t count,
                              std::uint32_t* dynamicOffsets) override;
    
    virtual void PushDebugGroup(const std::string &groupLabel) override
    {
        GFX_ASSERT(false, "unimplemented.");
    }
    
    virtual void PopDebugGroup() override
    {
        GFX_ASSERT(false, "unimplemented.");
    }
    
    virtual void InsertDebugMarker(const std::string &markerLabel) override
    {
        GFX_ASSERT(false, "unimplemented.");
    }

private:
    VKCommandBuffer* commandBuffer_ = nullptr;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKCOMPUTEPASSENCODER_H
