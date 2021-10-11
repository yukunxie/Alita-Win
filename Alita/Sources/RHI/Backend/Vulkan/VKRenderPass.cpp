//
// Created by realxie on 2019-10-04.
//

#include "VKRenderPass.h"
#include "VKTypes.h"
#include "VKRenderPipeline.h"
#include "VKBuffer.h"

#include <vector>
#include <array>
#include <vulkan/vulkan.h>

#include "GFX/xxhash64.h"

NS_GFX_BEGIN

VKRenderPass::VKRenderPass(VKDevice* device)
    : RenderPass(device)
{
}

void VKRenderPass::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    if (VK_NULL_HANDLE != vkRenderPass_)
    {
        vkDestroyRenderPass(VKDEVICE()->GetNative(), vkRenderPass_, nullptr);
        vkRenderPass_ = VK_NULL_HANDLE;
    }
    
    GFX_DISPOSE_END();
}

VKRenderPass::~VKRenderPass()
{
    Dispose();
}

bool VKRenderPass::Init(const RenderPassCacheQuery &query)
{
    std::array<VkAttachmentReference, kMaxColorAttachments * 2 + 1> attachmentRefs;
    
    // Contains the attachment description that will be chained in the create info
    std::array<VkAttachmentDescription, kMaxColorAttachments * 2 + 1> attachmentDescs = {};
    
    uint32_t attachmentCount = 0;
    for (std::uint32_t i = 0; i < query.GetColorAttachmentCount(); ++i)
    {
        if (!query.TestColorAttachment(i))
        {
            continue;
        }
        
        auto &attachmentRef = attachmentRefs[attachmentCount];
        auto &attachmentDesc = attachmentDescs[attachmentCount];
        
        attachmentRef.attachment = attachmentCount;
        attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        attachmentDesc.flags = 0;
        attachmentDesc.format = ToVulkanType(query.colorFormats[i]);
        attachmentDesc.samples = (VkSampleCountFlagBits) query.sampleCounts[i];
        attachmentDesc.loadOp = ToVulkanType(query.colorLoadOps[i]);
        attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDesc.initialLayout = query.TestResolvedColorAttachment(i)
            || !query.bIsSwapchainTextures[i] ?
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL :
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachmentDesc.finalLayout = query.TestResolvedColorAttachment(i)
                                     || !query.bIsSwapchainTextures[i] ?
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL :
                                     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        attachmentCount++;
    }
    uint32_t colorAttachmentCount = attachmentCount;
    
    // Resolve attachments
    std::uint32_t resolveAttachmentCount = 0;
    std::array<VkAttachmentReference, kMaxColorAttachments> resolveReferences;
    for (std::uint32_t i = 0; i < query.GetColorAttachmentCount(); ++i)
    {
        if (!query.TestColorAttachment(i) || !query.TestResolvedColorAttachment(i))
        {
            continue;
        }
        
        auto &attachmentRef = attachmentRefs[attachmentCount];
        auto &attachmentDesc = attachmentDescs[attachmentCount];
        
        attachmentRef.attachment = attachmentCount;
        attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        attachmentDesc.flags = 0;
        attachmentDesc.format = ToVulkanType(query.colorFormats[i]);
        attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        resolveReferences[resolveAttachmentCount].attachment = attachmentCount;
        resolveReferences[resolveAttachmentCount].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        resolveAttachmentCount++;
        attachmentCount++;
    }
    
    VkAttachmentReference* depthStencilAttachment = nullptr;
    if (query.bHasDepthStencil)
    {
        auto &attachmentRef = attachmentRefs[attachmentCount];
        auto &attachmentDesc = attachmentDescs[attachmentCount];
        
        depthStencilAttachment = &attachmentRefs[attachmentCount];
        
        attachmentRef.attachment = attachmentCount;
        attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        attachmentDesc.flags = 0;
        attachmentDesc.format = ToVulkanType(query.depthStencilFormat);
        attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDesc.loadOp = ToVulkanType(query.depthLoadOp);
        attachmentDesc.storeOp = ToVulkanType(query.depthStoreOp);
        attachmentDesc.stencilLoadOp = ToVulkanType(query.stencilLoadOp);
        attachmentDesc.stencilStoreOp = ToVulkanType(query.stencilStoreOp);
        attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        attachmentCount++;
    }
    
    // Create the VkSubpassDescription that will be chained in the VkRenderPassCreateInfo
    VkSubpassDescription subpassDesc;
    subpassDesc.flags = 0;
    subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.inputAttachmentCount = 0;
    subpassDesc.pInputAttachments = nullptr;
    subpassDesc.colorAttachmentCount = colorAttachmentCount;
    subpassDesc.pColorAttachments = attachmentRefs.data();
    subpassDesc.pResolveAttachments =
        resolveAttachmentCount > 0 ? resolveReferences.data() : nullptr;
    subpassDesc.pDepthStencilAttachment = depthStencilAttachment;
    subpassDesc.preserveAttachmentCount = 0;
    subpassDesc.pPreserveAttachments = nullptr;
    
     std::array<VkSubpassDependency, 2> dependencies;
    
     dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
     dependencies[0].dstSubpass = 0;
     dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
     dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
     dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
     dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
     dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    
     dependencies[1].srcSubpass = 0;
     dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
     dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
     dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
     dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
     dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
     dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    
    // Chain everything in VkRenderPassCreateInfo
    VkRenderPassCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = attachmentCount;
    createInfo.pAttachments = attachmentDescs.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpassDesc;
    createInfo.dependencyCount = dependencies.size();
    createInfo.pDependencies = dependencies.data();
    
    CALL_VK(vkCreateRenderPass(VKDEVICE()->GetNative(), &createInfo, nullptr, &vkRenderPass_));
    
    return VK_NULL_HANDLE != vkRenderPass_;
}

NS_GFX_END