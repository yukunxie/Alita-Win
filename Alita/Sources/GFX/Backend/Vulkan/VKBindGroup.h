//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_VKBINDGROUP_H
#define RHI_VKBINDGROUP_H

#include "VKDevice.h"
#include "VKBindGroupLayout.h"
#include "VKComputePipeline.h"
#include "VKCommandBuffer.h"

NS_GFX_BEGIN

class VKBuffer;

class VKBindGroup final : public BindGroup
{
protected:
    VKBindGroup(VKDevice* device);
    
    ~VKBindGroup();

public:
    
    bool Init(BindGroupDescriptor &descriptor);
    
    VkDescriptorSet GetNative()
    { return vkDescriptorSet_; }
    
    VKBindGroupLayout* GetBindGroupLayout()
    { return bindGroupLayout_.Get(); }
    
    virtual void Dispose() override;
    
    void UpdateDescriptorSetAsync();
    
    void TransImageLayoutToSampled(VKCommandBuffer *commandBuffer);

    std::vector<VKBuffer *> getBindingBuffers();

private:
    RHIObjectWrapper<VKBindGroupLayout> bindGroupLayout_;
    
    VkDescriptorSet vkDescriptorSet_ = VK_NULL_HANDLE;
    std::vector<BindGroupBinding> bindingResources_;
    
    bool hasUpdateDescriptorSet_ = false;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKBINDGROUP_H
