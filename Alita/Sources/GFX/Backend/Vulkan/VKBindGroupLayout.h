//
// Created by realxie on 2019-10-10.
//

#ifndef RHI_VKBINDGROUPLAYOUT_H
#define RHI_VKBINDGROUPLAYOUT_H

#include "VKDevice.h"

NS_GFX_BEGIN


class VKBindGroupLayout final : public BindGroupLayout
{
protected:
    VKBindGroupLayout(VKDevice* device);
    
    virtual ~VKBindGroupLayout();
    
public:
    
    bool Init(const BindGroupLayoutDescriptor &descriptor);
    
    
    VkDescriptorSetLayout GetNative() const
    { return vkBindGroupLayout_; }
    
    VkDescriptorType GetDescriptorType(std::uint32_t bindingPoint) const
    {
        for (auto &tp : vkDescriptorTypes_)
        {
            if (tp.first == bindingPoint)
            {
                return tp.second;
            }
        }
        GFX_ASSERT(false);
        return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
    
    std::uint32_t GetDynamicOffsetCount()
    { return dynamicOffsetCount_; }
    
    virtual void Dispose() override;

private:
    VkDescriptorSetLayout vkBindGroupLayout_ = VK_NULL_HANDLE;
    std::vector<std::pair<std::uint32_t, VkDescriptorType>> vkDescriptorTypes_;
    std::uint32_t dynamicOffsetCount_ = 0;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKBINDGROUPLAYOUT_H
