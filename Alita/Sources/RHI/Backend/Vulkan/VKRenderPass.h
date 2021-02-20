//
// Created by realxie on 2019-10-04.
//

#ifndef ALITA_VKRENDERPASS_H
#define ALITA_VKRENDERPASS_H

#include "VKDevice.h"

NS_RHI_BEGIN


class VKRenderPass : public RenderPass
{
public:
    VKRenderPass(VKDevice* device, const RenderPassCacheQuery &query);
    
    //    VKRenderPass(VKDevice* device, const RenderPassCreateInfo& createInfo);
    virtual ~VKRenderPass();
    
    VkRenderPass GetNative()
    { return vkRenderPass_; }

protected:
    //    void ParseAttachmentDescriptions(const RenderPassCreateInfo& createInfo, std::vector<VkAttachmentDescription>& descriptions);
    //    void ParseSubpassDescriptions(const RenderPassCreateInfo& createInfo, std::vector<VkSubpassDescription>& descriptions, std::vector<std::vector<VkAttachmentReference>>& references);
    //    void ParseAttachmentReferences(const std::vector<AttachmentReference>& attachments, std::vector<VkAttachmentReference>& attachmentReferences);

private:
    VKDevice* device_ = nullptr;
    VkRenderPass vkRenderPass_ = 0L;
};

NS_RHI_END


#endif //ALITA_VKRENDERPASS_H
