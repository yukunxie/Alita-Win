//
// Created by realxie on 2019-10-04.
//

#include "VKRenderPipeline.h"
#include "VKTypes.h"
#include "VKShader.h"
#include "VKBindGroupLayout.h"
#include "VKPipelineLayout.h"
#include "VKRenderPass.h"

#include <array>

NS_GFX_BEGIN

VKRenderPipeline::VKRenderPipeline(VKDevice* device)
    : RenderPipeline(device)
{
}

bool VKRenderPipeline::Init(const RenderPipelineDescriptor &descriptor)
{
    RenderPipeline::Init(descriptor);
    
    RHI_PTR_ASSIGN(pipelineLayout_, RHI_CAST(VKPipelineLayout * , descriptor.layout));
    
    VkPipelineShaderStageCreateInfo shaderStages[2];
    int shaderStageCount = 0;
    {
        const auto &vertexStageInfo = descriptor.vertexStage;
        const auto &fragmentStageInfo = descriptor.fragmentStage;
        
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].flags = 0;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = RHI_CAST(const VKShader*, vertexStageInfo.shader)->GetNative();
        shaderStages[0].pName = vertexStageInfo.entryPoint.c_str();
        shaderStages[0].pSpecializationInfo = nullptr;
        shaderStageCount++;
        
        if (fragmentStageInfo.shader)
        {
            shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[1].pNext = nullptr;
            shaderStages[1].flags = 0;
            shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderStages[1].module = RHI_CAST(const VKShader*, fragmentStageInfo.shader)->GetNative();
            shaderStages[1].pName = fragmentStageInfo.entryPoint.c_str();
            shaderStages[1].pSpecializationInfo = nullptr;
            shaderStageCount++;
        }
        
    }
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    {
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.pNext = nullptr;
        inputAssembly.flags = 0;
        inputAssembly.topology = ToVulkanType(descriptor.primitiveTopology);
        // Primitive restart is always enabled in Dawn (because of Metal)
        inputAssembly.primitiveRestartEnable = VK_FALSE;
    }
    
    // A dummy viewport/scissor info. The validation layers force use to provide at least one
    // scissor and one viewport here, even if we choose to make them dynamic.
    VkViewport viewportDesc;
    {
        viewportDesc.x = 0.0f;
        viewportDesc.y = 0.0f;
        viewportDesc.width = 1.0f;
        viewportDesc.height = 1.0f;
        viewportDesc.minDepth = 0.0f;
        viewportDesc.maxDepth = 1.0f;
    }
    
    VkRect2D scissorRect;
    {
        scissorRect.offset.x = 0;
        scissorRect.offset.y = 0;
        scissorRect.extent.width = 1;
        scissorRect.extent.height = 1;
    }
    
    VkPipelineViewportStateCreateInfo viewport;
    {
        viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport.pNext = nullptr;
        viewport.flags = 0;
        viewport.viewportCount = 1;
        viewport.pViewports = &viewportDesc;
        viewport.scissorCount = 1;
        viewport.pScissors = &scissorRect;
    }
    
    
    VkPipelineRasterizationStateCreateInfo rasterization;
    {
        rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.pNext = nullptr;
        rasterization.flags = 0;
        rasterization.depthClampEnable = VK_FALSE;
        rasterization.rasterizerDiscardEnable = VK_FALSE;
        rasterization.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization.cullMode = ToVulkanType(descriptor.rasterizationState.cullMode);
        rasterization.frontFace = ToVulkanType(descriptor.rasterizationState.frontFace);
        rasterization.depthBiasEnable = descriptor.rasterizationState.depthBias;
        rasterization.depthBiasConstantFactor = 0.0f;
        rasterization.depthBiasClamp = descriptor.rasterizationState.depthBiasClamp;
        rasterization.depthBiasSlopeFactor = descriptor.rasterizationState.depthBiasSlopeScale;
        rasterization.lineWidth = 1.0f;
    }
    
    VkPipelineMultisampleStateCreateInfo multisample;
    {
        multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample.pNext = nullptr;
        multisample.flags = 0;
        multisample.rasterizationSamples = (VkSampleCountFlagBits) descriptor.sampleCount;
        multisample.sampleShadingEnable = VK_FALSE;
        multisample.minSampleShading = 0.0f;
        multisample.pSampleMask = nullptr;
        multisample.alphaToCoverageEnable = VK_FALSE;
        multisample.alphaToOneEnable = VK_FALSE;
    }
    
    VkPipelineColorBlendAttachmentState colorBlendAttachments[kMaxColorAttachments];
    
    std::uint32_t colorAttachmentCount = std::min(kMaxColorAttachments,
                                                  (std::uint32_t) descriptor.colorStates.size());
    
    auto _checkBlendDisabled = [](const BlendDescriptor& desc) -> bool {
        bool ret = desc.dstFactor == BlendFactor::ZERO;
        ret = ret && (desc.srcFactor == BlendFactor::ONE);
        ret = ret && (desc.operation == BlendOp::ADD);
        return ret;
    };
    
    for (std::uint32_t idx = 0; idx < colorAttachmentCount; ++idx)
    {
        auto &colorBlendAttachment = colorBlendAttachments[idx];
        const ColorStateDescriptor &colorState = descriptor.colorStates[idx];
    
        VkBool32 blendEnable = VK_TRUE;
        
        if (_checkBlendDisabled(colorState.colorBlend) && _checkBlendDisabled(colorState.alphaBlend))
        {
            blendEnable = VK_FALSE;
        }
        colorBlendAttachment.colorWriteMask = colorState.writeMask;
        
        if (blendEnable)
        {
            colorBlendAttachment.blendEnable = VK_TRUE;
            colorBlendAttachment.srcColorBlendFactor = ToVulkanType(colorState.colorBlend.srcFactor);
            colorBlendAttachment.dstColorBlendFactor = ToVulkanType(colorState.colorBlend.dstFactor);
            colorBlendAttachment.colorBlendOp = ToVulkanType(colorState.colorBlend.operation);
            colorBlendAttachment.srcAlphaBlendFactor = ToVulkanType(colorState.alphaBlend.srcFactor);
            colorBlendAttachment.dstAlphaBlendFactor = ToVulkanType(colorState.alphaBlend.dstFactor);
            colorBlendAttachment.alphaBlendOp = ToVulkanType(colorState.alphaBlend.operation);
        }
        else
        {
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }
    }
    
    VkPipelineColorBlendStateCreateInfo colorBlending;
    {
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext = nullptr;
        colorBlending.flags = 0;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = colorAttachmentCount;
        colorBlending.pAttachments = colorBlendAttachments;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
    };
    
    // Tag all state as dynamic but stencil masks.
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,
        VK_DYNAMIC_STATE_DEPTH_BIAS,
        VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        VK_DYNAMIC_STATE_STENCIL_REFERENCE,
        //// VK_DYNAMIC_STATE_DEPTH_BOUNDS,
    };
    
    VkPipelineDynamicStateCreateInfo dynamic;
    {
        dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic.pNext = nullptr;
        dynamic.flags = 0;
        dynamic.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
        dynamic.pDynamicStates = dynamicStates;
    }
    
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    
    for (size_t i = 0; i < descriptor.vertexState.vertexBuffers.size(); ++i)
    {
        const auto &vertexBuffer = descriptor.vertexState.vertexBuffers[i];
        // Notice, vertex binding must be start with 0
        VkVertexInputBindingDescription bindingDescription;
        {
            bindingDescription.binding = bindingDescriptions.size();
            bindingDescription.inputRate = ToVulkanType(vertexBuffer.stepMode);
            bindingDescription.stride = vertexBuffer.arrayStride;
        }
        bindingDescriptions.push_back(bindingDescription);
        
        for (size_t j = 0; j < vertexBuffer.attributes.size(); ++j)
        {
            const auto &attriDescriptor = vertexBuffer.attributes[j];
            VkVertexInputAttributeDescription attributeDescription;
            {
                attributeDescription.binding = bindingDescriptions.size() - 1;
                attributeDescription.location = attriDescriptor.shaderLocation;
                attributeDescription.offset = attriDescriptor.offset;
                attributeDescription.format = ToVulkanType(attriDescriptor.format);
            }
            attributeDescriptions.push_back(attributeDescription);
        }
    }
    
    VkPipelineVertexInputStateCreateInfo vertexInputCreateDescription;
    {
        vertexInputCreateDescription.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputCreateDescription.pNext = nullptr;
        vertexInputCreateDescription.flags = 0;
        vertexInputCreateDescription.vertexBindingDescriptionCount = (std::uint32_t) bindingDescriptions.size();
        vertexInputCreateDescription.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInputCreateDescription.vertexAttributeDescriptionCount = (std::uint32_t) attributeDescriptions.size();
        vertexInputCreateDescription.pVertexAttributeDescriptions = attributeDescriptions.data();
    };
    
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
    {
        const DepthStencilStateDescriptor &dsDescriptor = descriptor.depthStencilState;
        
        depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateCreateInfo.pNext = nullptr;
        depthStencilStateCreateInfo.flags = 0;
        depthStencilStateCreateInfo.depthTestEnable =
            dsDescriptor.depthCompare != CompareFunction::ALWAYS ? VK_TRUE : VK_FALSE;
        depthStencilStateCreateInfo.depthWriteEnable = ToVulkanType(
            dsDescriptor.depthWriteEnabled);
        depthStencilStateCreateInfo.depthCompareOp = ToVulkanType(dsDescriptor.depthCompare);
        
        depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.minDepthBounds = 0.0f;
        depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
        
        depthStencilStateCreateInfo.stencilTestEnable = 
            dsDescriptor.stencilFront.compare != CompareFunction::ALWAYS ||
            dsDescriptor.stencilBack.compare != CompareFunction::ALWAYS;
        
        depthStencilStateCreateInfo.front.failOp = ToVulkanType(dsDescriptor.stencilFront.failOp);
        depthStencilStateCreateInfo.front.passOp = ToVulkanType(dsDescriptor.stencilFront.passOp);
        depthStencilStateCreateInfo.front.depthFailOp = ToVulkanType(
            dsDescriptor.stencilFront.depthFailOp);
        depthStencilStateCreateInfo.front.compareOp = ToVulkanType(
            dsDescriptor.stencilFront.compare);
        depthStencilStateCreateInfo.front.compareMask = dsDescriptor.stencilReadMask;
        depthStencilStateCreateInfo.front.writeMask = dsDescriptor.stencilWriteMask;
        depthStencilStateCreateInfo.front.reference = 0;
        
        depthStencilStateCreateInfo.back.failOp = ToVulkanType(dsDescriptor.stencilBack.failOp);
        depthStencilStateCreateInfo.back.passOp = ToVulkanType(dsDescriptor.stencilBack.passOp);
        depthStencilStateCreateInfo.back.depthFailOp = ToVulkanType(
            dsDescriptor.stencilBack.depthFailOp);
        depthStencilStateCreateInfo.back.compareOp = ToVulkanType(dsDescriptor.stencilBack.compare);
        depthStencilStateCreateInfo.back.compareMask = dsDescriptor.stencilReadMask;
        depthStencilStateCreateInfo.back.writeMask = dsDescriptor.stencilWriteMask;
        depthStencilStateCreateInfo.back.reference = 0;
    }
    
    {
        RenderPassCacheQuery query;
        std::uint32_t attachmentCount = 0;
        for (size_t i = 0; i < descriptor.colorStates.size(); ++i)
        {
            auto &colorState = descriptor.colorStates[i];
            query.SetColor(attachmentCount,
                           colorState.format,
                           LoadOp::CLEAR,
                           StoreOp::STORE,
                           descriptor.sampleCount > 1,
                           descriptor.sampleCount);
            attachmentCount++;
        }
        
        if (descriptor.hasDepthStencilState)
        {
            const auto &depthStencilState = descriptor.depthStencilState;
            query.SetDepthStencil(depthStencilState.format,
                                  LoadOp::CLEAR,
                                  StoreOp::STORE,
                                  LoadOp::CLEAR,
                                  StoreOp::STORE);
        }
        
        RHI_PTR_ASSIGN(renderPass_, VKDEVICE()->GetOrCreateRenderPass(query));
    }
    
    // The create info chains in a bunch of things created on the stack here or inside state
    // objects.
    VkGraphicsPipelineCreateInfo createInfo;
    {
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.stageCount = shaderStageCount;
        createInfo.pStages = shaderStages;
        createInfo.pVertexInputState = &vertexInputCreateDescription;
        createInfo.pInputAssemblyState = &inputAssembly;
        createInfo.pTessellationState = nullptr;
        createInfo.pViewportState = &viewport;
        createInfo.pRasterizationState = &rasterization;
        createInfo.pMultisampleState = &multisample;
        createInfo.pDepthStencilState = &depthStencilStateCreateInfo;
        createInfo.pColorBlendState = &colorBlending;
        createInfo.pDynamicState = &dynamic;
        createInfo.layout = RHI_CAST(const VKPipelineLayout*, descriptor.layout)->GetNative();
        createInfo.renderPass = renderPass_->GetNative();
        createInfo.subpass = 0;
        createInfo.basePipelineHandle = VK_NULL_HANDLE;
        createInfo.basePipelineIndex = -1;
    }
    
    CALL_VK(
        vkCreateGraphicsPipelines(VKDEVICE()->GetNative(), VK_NULL_HANDLE, 1, &createInfo,
                                    nullptr,
                                    &vkGraphicsPipeline_));
    
    return VK_NULL_HANDLE != vkGraphicsPipeline_;
}

VkPipelineLayout VKRenderPipeline::GetPipelineLayout() const
{
    return pipelineLayout_->GetNative();
}

void VKRenderPipeline::Dispose()
{
    RHI_DISPOSE_BEGIN();
    
    if (vkGraphicsPipeline_)
    {
        vkDestroyPipeline(VKDEVICE()->GetNative(), vkGraphicsPipeline_, nullptr);
        vkGraphicsPipeline_ = VK_NULL_HANDLE;
    }
    RHI_SAFE_RELEASE(pipelineLayout_);
    RHI_SAFE_RELEASE(renderPass_);
    
    RHI_DISPOSE_END();
}

VKRenderPipeline::~VKRenderPipeline()
{
    Dispose();
}

NS_GFX_END
