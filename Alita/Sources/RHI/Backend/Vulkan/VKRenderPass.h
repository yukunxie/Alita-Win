//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_VKRENDERPASS_H
#define RHI_VKRENDERPASS_H

#include "VKDevice.h"

NS_RHI_BEGIN


class VKRenderPass final : public RenderPass
{
protected:
    VKRenderPass(VKDevice* device);
    
    virtual ~VKRenderPass();

public:
    
    bool Init(const RenderPassCacheQuery &query);
    
    VkRenderPass GetNative()
    { return vkRenderPass_; }
    
    virtual void Dispose() override;

private:
    VkRenderPass vkRenderPass_ = VK_NULL_HANDLE;
    
    friend class VKDevice;
};

NS_RHI_END


#endif //RHI_VKRENDERPASS_H
