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

#include "../../Include/xxhash64.h"

NS_RHI_BEGIN

VKRenderPass::VKRenderPass(VKDevice* device, const RenderPassCacheQuery &query)
    : device_(device)
{
    std::array<VkAttachmentReference, kMaxColorAttachments + 1> attachmentRefs;
    
    // Contains the attachment description that will be chained in the create info
    std::array<VkAttachmentDescription, kMaxColorAttachments + 1> attachmentDescs = {};
    
    uint32_t attachmentCount = 0;
    for (size_t i = 0; i < query.colorMask.size(); ++i)
    {
        if (false == query.colorMask.test(i))
        {
            continue;
        }
        
        auto &attachmentRef = attachmentRefs[attachmentCount];
        auto &attachmentDesc = attachmentDescs[attachmentCount];
        
        attachmentRef.attachment = attachmentCount;
        attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        attachmentDesc.flags = 0;
        attachmentDesc.format = GetVkFormat(query.colorFormats[i]);
        attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDesc.loadOp = GetLoadOp(query.colorLoadOp[i]);
        attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        attachmentCount++;
    }
    uint32_t colorAttachmentCount = attachmentCount;
    
    VkAttachmentReference* depthStencilAttachment = nullptr;
    if (query.hasDepthStencil)
    {
        auto &attachmentRef = attachmentRefs[attachmentCount];
        auto &attachmentDesc = attachmentDescs[attachmentCount];
        
        depthStencilAttachment = &attachmentRefs[attachmentCount];
        
        attachmentRef.attachment = attachmentCount;
        attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        attachmentDesc.flags = 0;
        attachmentDesc.format = GetVkFormat(query.depthStencilFormat);
        attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDesc.loadOp = GetLoadOp(query.depthLoadOp);
        attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDesc.stencilLoadOp = GetLoadOp(query.stencilLoadOp);
        attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
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
    subpassDesc.pResolveAttachments = nullptr;
    subpassDesc.pDepthStencilAttachment = depthStencilAttachment;
    subpassDesc.preserveAttachmentCount = 0;
    subpassDesc.pPreserveAttachments = nullptr;
    
    // Chain everything in VkRenderPassCreateInfo
    VkRenderPassCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = attachmentCount;
    createInfo.pAttachments = attachmentDescs.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpassDesc;
    createInfo.dependencyCount = 0;
    createInfo.pDependencies = nullptr;
    
    CALL_VK(vkCreateRenderPass(device->GetDevice(), &createInfo, nullptr, &vkRenderPass_));
    
}

//VKRenderPass::VKRenderPass(VKDevice* device, const RenderPassCreateInfo& createInfo)
//{
//    // Setup descriptions for attachments.
//    std::vector<VkAttachmentDescription> attachmentDescriptions(createInfo.attachments.size());
//    ParseAttachmentDescriptions(createInfo, attachmentDescriptions);
//
//    // Setup descriptions for subpasses.
//    std::vector<VkSubpassDescription> subpassDescriptions(createInfo.subpasses.size());
//    std::vector<std::vector<VkAttachmentReference>> attachReferences;
//    ParseSubpassDescriptions(createInfo, subpassDescriptions, attachReferences);
//
//    // TODO realxie configurable
//    VkAttachmentReference depthAttachmentRef = {};
//    depthAttachmentRef.attachment = 1;
//    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//    subpassDescriptions[0].pDepthStencilAttachment = &depthAttachmentRef;
//
//    VkSubpassDependency dependency = {};
//    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//    dependency.dstSubpass = 0;
//    dependency.srcStageMask = 0;
//    dependency.dstStageMask = 0;
//
//    VkRenderPassCreateInfo renderPassInfo = {
//            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
//            .pNext = nullptr,
//            .attachmentCount = (std::uint32_t)attachmentDescriptions.size(),
//            .pAttachments = attachmentDescriptions.data(),
//            .subpassCount = (std::uint32_t)subpassDescriptions.size(),
//            .pSubpasses = subpassDescriptions.data(),
//            .dependencyCount = 1,
//            .pDependencies = &dependency
//    };
//
//    CALL_VK(vkCreateRenderPass(device->GetDevice(), &renderPassInfo, nullptr, &vkRenderPass_));
//}

VKRenderPass::~VKRenderPass()
{
    vkDestroyRenderPass(device_->GetDevice(), vkRenderPass_, nullptr);
}

//void VKRenderPass::ParseAttachmentDescriptions(const RenderPassCreateInfo& createInfo, std::vector<VkAttachmentDescription>& descriptions)
//{
//    descriptions.resize(createInfo.attachments.size());
//    for (size_t i = 0; i < createInfo.attachments.size(); ++i)
//    {
//        const AttachmentDescription& desc = createInfo.attachments[i];
//        VkAttachmentDescription& target = descriptions[i];
//        target.format = ToVkFormat(desc.format);
//        target.samples = ToVkSampleCountFlagBits(desc.samples);
//        target.loadOp = ToVkAttachmentLoadOp(desc.loadOp);
//        target.storeOp = ToVkAttachmentStoreOp(desc.storeOp);
//        target.stencilLoadOp = ToVkAttachmentLoadOp(desc.stencilLoadOp);
//        target.stencilStoreOp = ToVkAttachmentStoreOp(desc.stencilStoreOp);
//        target.initialLayout = ToVkImageLayout(desc.initialLayout);
//        target.finalLayout = ToVkImageLayout(desc.finalLayout);
//    }
//}
//
//void VKRenderPass::ParseSubpassDescriptions(const RenderPassCreateInfo& createInfo
//        , std::vector<VkSubpassDescription>& descriptions
//        , std::vector<std::vector<VkAttachmentReference>>& references
//        )
//{
//    descriptions.resize(createInfo.subpasses.size());
//    references.resize(descriptions.size());
//
//    for (size_t i = 0; i < createInfo.subpasses.size(); ++i)
//    {
//        const SubpassDescription& sp = createInfo.subpasses[i];
//        RHI_ASSERT(sp.inputAttachments.size() == 0);
//        RHI_ASSERT(sp.resolveAttachments.size() == 0);
//        RHI_ASSERT(sp.preserveAttachments.size() == 0);
//
//        VkSubpassDescription& description = descriptions[i];
//        description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//        description.flags = 0;
//        description.inputAttachmentCount = 0;
//        description.pInputAttachments = nullptr;
//
//        ParseAttachmentReferences(sp.colorAttachments, references[i]);
//        description.colorAttachmentCount = references[i].size();
//        description.pColorAttachments = references[i].data();
//        description.pResolveAttachments = nullptr;
//        description.pDepthStencilAttachment = nullptr;
//        description.preserveAttachmentCount = 0;
//        description.pPreserveAttachments = nullptr;
//    }
//}
//
//void VKRenderPass::ParseAttachmentReferences(const std::vector<AttachmentReference>& attachments, std::vector<VkAttachmentReference>& attachmentReferences)
//{
//    attachmentReferences.resize(attachments.size());
//    for (size_t i = 0; i < attachments.size(); ++i)
//    {
//        attachmentReferences[i].attachment = attachments[i].attachment;
//        attachmentReferences[i].layout = ToVkImageLayout(attachments[i].layout);
//    }
//}

NS_RHI_END