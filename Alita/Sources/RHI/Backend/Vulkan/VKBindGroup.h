//
// Created by realxie on 2019-10-10.
//

#ifndef ALITA_VKBINDGROUP_H
#define ALITA_VKBINDGROUP_H

#include "VKDevice.h"
#include "VKBindGroupLayout.h"

NS_RHI_BEGIN

class VKBindGroup : public BindGroup
{
protected:
    VKBindGroup() = default;
    bool Init(VKDevice* device, const BindGroupDescriptor& descriptor);
    
public:
    static VKBindGroup* Create(VKDevice* device, const BindGroupDescriptor& descriptor);
    
    ~VKBindGroup();

    VkDescriptorSet GetNative() const {return vkDescriptorSet_;}

    void WriteToGPU() const;

    void BindToCommandBuffer(std::uint32_t index, VkCommandBuffer vkCommandBuffer, VkPipelineLayout vkPipelineLayout) const;

private:
    VkDevice                    vkDevice_           = nullptr;
    VkDescriptorPool            vkDescriptorPool_   = 0L;
    VkDescriptorSet             vkDescriptorSet_    = 0L;

    std::vector<BindGroupBinding> bindingResources_;
};

NS_RHI_END


#endif //ALITA_VKBINDGROUP_H
