//
// Created by realxie on 2019-10-07.
//

#ifndef RHI_VKSAMPLER_H
#define RHI_VKSAMPLER_H

#include "VKDevice.h"

NS_GFX_BEGIN

class VKSampler final : public Sampler
{
protected:
    VKSampler(VKDevice* device);
    
    virtual ~VKSampler();
    
public:
    
    bool Init(const SamplerDescriptor &descriptor);
    
    VkSampler GetNative() const
    { return vkSampler_; }
    
    virtual void Dispose() override;

private:
    VkSampler vkSampler_ = VK_NULL_HANDLE;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKSAMPLER_H
