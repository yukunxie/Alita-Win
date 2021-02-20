//
// Created by realxie on 2019-10-10.
//

#ifndef ALITA_VKPIPELINELAYOUT_H
#define ALITA_VKPIPELINELAYOUT_H

#include "VKDevice.h"
#include "VKBindGroupLayout.h"

NS_RHI_BEGIN

class VKPipelineLayout : public PipelineLayout
{
protected:
    VKPipelineLayout() = default;
    
    bool Init(VKDevice* device, const PipelineLayoutDescriptor &descriptor);

public:
    static VKPipelineLayout* Create(VKDevice* device, const PipelineLayoutDescriptor &descriptor);
    
    ~VKPipelineLayout();
    
    VkPipelineLayout GetNative() const
    { return vkPipelineLayout_; }

private:
    VkDevice vkDevice_ = nullptr;
    VkPipelineLayout vkPipelineLayout_ = 0;
};

NS_RHI_END


#endif //ALITA_VKPIPELINELAYOUT_H
