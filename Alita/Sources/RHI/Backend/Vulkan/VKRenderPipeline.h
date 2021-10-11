//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_VKRENDERPIPELINE_H
#define RHI_VKRENDERPIPELINE_H

#include "VKDevice.h"

NS_GFX_BEGIN

class VKRenderPass;

class VKPipelineLayout;

class VKRenderPipeline final : public RenderPipeline
{
protected:
    VKRenderPipeline(VKDevice* device);
    
    virtual ~VKRenderPipeline();

public:
    
    bool Init(const RenderPipelineDescriptor &descriptor);
    
    VkPipeline GetNative() const
    { return vkGraphicsPipeline_; }
    
    VkPipelineLayout GetPipelineLayout() const;
    
    virtual void Dispose() override;
    
    VkPipelineBindPoint GetPipelineBindPoint()
    {
        return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
    }

private:
    VKPipelineLayout* pipelineLayout_ = nullptr;
    VkPipeline vkGraphicsPipeline_ = VK_NULL_HANDLE;
    VKRenderPass* renderPass_ = nullptr;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKRENDERPIPELINE_H
