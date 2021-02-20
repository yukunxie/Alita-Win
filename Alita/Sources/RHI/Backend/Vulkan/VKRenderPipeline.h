//
// Created by realxie on 2019-10-04.
//

#ifndef ALITA_VKRENDERPIPELINE_H
#define ALITA_VKRENDERPIPELINE_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKRenderPass;

class VKRenderPipeline : public RenderPipeline
{
public:
    VKRenderPipeline(VKDevice* device, const RenderPipelineDescriptor &descriptor);
    
    virtual ~VKRenderPipeline();
    
    VkPipeline GetNative() const
    { return vkGraphicsPipeline_; }
    
    VkPipelineLayout GetPipelineLayout() const
    { return vkPipelineLayout_; }

private:

private:
    VkPipeline vkGraphicsPipeline_ = 0;
    VkPipelineLayout vkPipelineLayout_ = 0;
    VKRenderPass* renderPass_ = nullptr;
    
    friend class VKDevice;
};

NS_RHI_END


#endif //ALITA_VKRENDERPIPELINE_H
