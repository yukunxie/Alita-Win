//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_VKPIPELINELAYOUT_H
#define RHI_VKPIPELINELAYOUT_H

#include "VKDevice.h"
#include "VKBindGroupLayout.h"

NS_RHI_BEGIN

class VKPipelineLayout final : public PipelineLayout
{
protected:
    VKPipelineLayout(VKDevice* device);
    
    ~VKPipelineLayout();
    
public:
    
    bool Init(const PipelineLayoutDescriptor &descriptor);
    
    VkPipelineLayout GetNative() const
    { return vkPipelineLayout_; }
    
    virtual void Dispose() override;

private:
    VkPipelineLayout vkPipelineLayout_ = VK_NULL_HANDLE;
    
    friend class VKDevice;
};

NS_RHI_END


#endif //RHI_VKPIPELINELAYOUT_H
