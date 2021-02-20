//
// Created by realxie on 2019-10-10.
//

#ifndef ALITA_VKBINDGROUPLAYOUT_H
#define ALITA_VKBINDGROUPLAYOUT_H

#include "VKDevice.h"

NS_RHI_BEGIN


class VKBindGroupLayout : public BindGroupLayout
{
protected:
    VKBindGroupLayout() = default;
    bool Init(VKDevice* device, const BindGroupLayoutDescriptor& descriptor);
    
public:
    static VKBindGroupLayout* Create(VKDevice* device, const BindGroupLayoutDescriptor& descriptor);
    
public:
    virtual ~VKBindGroupLayout();

    VkDescriptorSetLayout GetNative() const {return vkBindGroupLayout_;}

private:
    VkDevice         vkDevice_          = nullptr;
    VkDescriptorSetLayout vkBindGroupLayout_  = 0L;
};

NS_RHI_END


#endif //ALITA_VKBINDGROUPLAYOUT_H
