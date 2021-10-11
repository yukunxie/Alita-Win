//
// Created by realxie on 2020-01-19.
//

#ifndef RHI_VKCOMPUTEPIPELINE_H
#define RHI_VKCOMPUTEPIPELINE_H

#include "VKDevice.h"
#include "VKPipelineLayout.h"

NS_GFX_BEGIN

class VKComputePipeline final : public ComputePipeline
{
protected:
    VKComputePipeline(VKDevice* device);
    
    ~VKComputePipeline();

public:
    
    bool Init(const ComputePipelineDescriptor &descriptor);
    
    virtual void Dispose() override;
    
    VkPipeline GetNative()
    { return vkComputePipeline_; }
    
    VkPipelineLayout GetPipelineLayout() const;
    
    VkPipelineBindPoint GetPipelineBindPoint()
    {
        return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE;
    }

private:
    VKPipelineLayout* pipelineLayout_ = nullptr;
    VkPipeline vkComputePipeline_ = VK_NULL_HANDLE;
    VKRenderPass* renderPass_ = nullptr;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKCOMPUTEPIPELINE_H
