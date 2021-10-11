//
// Created by realxie on 2019-12-25.
//

#include "VKFramebuffer.h"
#include "VKRenderPass.h"
#include "VKTextureView.h"
#include <vector>

NS_GFX_BEGIN

VKFramebuffer::VKFramebuffer(VKDevice* device)
    : Framebuffer(device)
{
}

bool VKFramebuffer::Init(const FramebufferCacheQuery &query)
{
    colorAttachments_.fill(nullptr);
    
    renderPass_ = query.renderPass;
    GFX_SAFE_RETAIN(renderPass_);
    
    for (std::uint32_t i = 0; i < kMaxColorAttachments; ++i)
    {
        if (query.attachments[i])
        {
            colorAttachments_[colorAttachmentCount_] = query.attachments[i];
            GFX_SAFE_RETAIN(colorAttachments_[colorAttachmentCount_]);
            ++colorAttachmentCount_;
        }
    }
    
    extent2D_ = {query.width, query.height};
    
    return true;
}

void VKFramebuffer::Prepare()
{
    if (vkFramebuffer_)
    {
        return;
    }
    
    std::array<VkImageView, kMaxColorAttachments> attachments;
    attachments.fill(VK_NULL_HANDLE);
    for (std::uint32_t i = 0; i < colorAttachmentCount_; ++i)
    {
        attachments[i] = colorAttachments_[i]->GetNative();
        hasSwapChainImages_ |= colorAttachments_[i]->GetTexture()->IsSwapchainImage();
    }
    
    VkFramebufferCreateInfo createInfo;
    {
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        createInfo.renderPass = renderPass_->GetNative();
        createInfo.layers = 1;
        createInfo.attachmentCount = colorAttachmentCount_;
        createInfo.pAttachments = attachments.data();
        createInfo.width = extent2D_.width;
        createInfo.height = extent2D_.height;
    };
    CALL_VK(vkCreateFramebuffer(VKDEVICE()->GetNative(), &createInfo, nullptr, &vkFramebuffer_));
    
    LOGI("CreateFramebuffer done %p", (void*)vkFramebuffer_);
    
}

void VKFramebuffer::Invalidate()
{
    if (vkFramebuffer_)
    {
        vkDestroyFramebuffer(VKDEVICE()->GetNative(), vkFramebuffer_, nullptr);
        vkFramebuffer_ = VK_NULL_HANDLE;
    }
}

void VKFramebuffer::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    if (vkFramebuffer_)
    {
        vkDestroyFramebuffer(VKDEVICE()->GetNative(), vkFramebuffer_, nullptr);
        vkFramebuffer_ = VK_NULL_HANDLE;
    }
    
    for (std::uint32_t i = 0; i < colorAttachmentCount_; ++i)
    {
        GFX_SAFE_RELEASE(colorAttachments_[i]);
    }
    GFX_SAFE_RELEASE(renderPass_);
    
    GFX_DISPOSE_END();
}

VKFramebuffer::~VKFramebuffer()
{
    Dispose();
}


NS_GFX_END