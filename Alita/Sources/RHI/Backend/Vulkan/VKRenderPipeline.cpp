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

NS_RHI_BEGIN

VKRenderPipeline::VKRenderPipeline(VKDevice* device, const RenderPipelineDescriptor &descriptor)
{
    vkPipelineLayout_ = RHI_CAST(const VKPipelineLayout*, descriptor.layout)->GetNative();
    
    VkPipelineShaderStageCreateInfo shaderStages[2];
    {
        const auto &vertexStageInfo = descriptor.vertexStage;
        const auto &fragmentStageInfo = descriptor.fragmentStage;
        
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].flags = 0;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = RHI_CAST(const VKShader*, vertexStageInfo.shaderModule)->GetNative();
        shaderStages[0].pName = vertexStageInfo.entryPoint.c_str();
        shaderStages[0].pSpecializationInfo = nullptr;
        
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].flags = 0;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = RHI_CAST(const VKShader*, fragmentStageInfo.shaderModule)->GetNative();
        shaderStages[1].pName = fragmentStageInfo.entryPoint.c_str();
        shaderStages[1].pSpecializationInfo = nullptr;
    }
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    {
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.pNext = nullptr;
        inputAssembly.flags = 0;
        inputAssembly.topology = GetPrimitiveTopology(descriptor.primitiveTopology);
        // Primitive restart is always enabled in Dawn (because of Metal)
        inputAssembly.primitiveRestartEnable = VK_TRUE;
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
        rasterization.cullMode = VK_CULL_MODE_NONE;
        rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization.depthBiasEnable = VK_FALSE;
        rasterization.depthBiasConstantFactor = 0.0f;
        rasterization.depthBiasClamp = 0.0f;
        rasterization.depthBiasSlopeFactor = 0.0f;
        rasterization.lineWidth = 1.0f;
    }
    
    VkPipelineMultisampleStateCreateInfo multisample;
    {
        multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample.pNext = nullptr;
        multisample.flags = 0;
        multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisample.sampleShadingEnable = VK_FALSE;
        multisample.minSampleShading = 0.0f;
        multisample.pSampleMask = nullptr;
        multisample.alphaToCoverageEnable = VK_FALSE;
        multisample.alphaToOneEnable = VK_FALSE;
    }
    
    // TODO realxie configurable
    
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    {
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
        
        // colorBlendAttachment.blendEnable = VK_TRUE;
        // colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        // colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        // colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        // colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        // colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        // colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }
    
    VkPipelineColorBlendStateCreateInfo colorBlending;
    {
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext = nullptr;
        colorBlending.flags = 0;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional
    };
    
    // Tag all state as dynamic but stencil masks.
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH, VK_DYNAMIC_STATE_DEPTH_BIAS,
        VK_DYNAMIC_STATE_BLEND_CONSTANTS, VK_DYNAMIC_STATE_DEPTH_BOUNDS,
        VK_DYNAMIC_STATE_STENCIL_REFERENCE,
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
    
    for (const auto &vertexBuffer: descriptor.vertexInput.vertexBuffers)
    {
        // Notice, vertex binding must be start with 0
        VkVertexInputBindingDescription bindingDescription;
        {
            bindingDescription.binding = bindingDescriptions.size();
            bindingDescription.inputRate = GetVertexInputRate(vertexBuffer.stepMode);
            bindingDescription.stride = vertexBuffer.stride;
        }
        bindingDescriptions.push_back(bindingDescription);
        
        for (const auto &attriDescriptor : vertexBuffer.attributeSet)
        {
            VkVertexInputAttributeDescription attributeDescription;
            {
                attributeDescription.binding = bindingDescriptions.size() - 1;
                attributeDescription.location = attriDescriptor.shaderLocation;
                attributeDescription.offset = attriDescriptor.offset;
                attributeDescription.format = GetVkFormat(attriDescriptor.format);
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
        if (descriptor.depthStencilState.has_value())
        {
            const DepthStencilStateDescriptor &dsDescriptor = descriptor.depthStencilState.value();
            
            depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilStateCreateInfo.pNext = nullptr;
            depthStencilStateCreateInfo.flags = 0;
            depthStencilStateCreateInfo.depthTestEnable =
                dsDescriptor.depthCompare != CompareFunction::ALWAYS ? VK_TRUE : VK_FALSE;
            depthStencilStateCreateInfo.depthWriteEnable = GetVkBoolean(
                dsDescriptor.depthWriteEnabled);
            depthStencilStateCreateInfo.depthCompareOp = GetCompareOp(dsDescriptor.depthCompare);
            
            depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
            depthStencilStateCreateInfo.minDepthBounds = 0.0f; // Optional
            depthStencilStateCreateInfo.maxDepthBounds = 1.0f; // Optional
            
            // TODO realxie
            depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
            depthStencilStateCreateInfo.front = {}; // Optional
            depthStencilStateCreateInfo.back = {}; // Optional
        }
        else
        {
            depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilStateCreateInfo.flags = 0;
            depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
            depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
            depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
        }
    }
    
    {
        RenderPassCacheQuery query;
        std::uint32_t attachmentCount = 0;
        for (auto &colorState : descriptor.colorStates)
        {
            query.SetColor(attachmentCount, colorState.format, LoadOp::LOAD);
            attachmentCount++;
        }
        
        if (descriptor.depthStencilState.has_value())
        {
            const auto &depthStencilState = descriptor.depthStencilState.value();
            query.SetDepthStencil(depthStencilState.format, LoadOp::LOAD, LoadOp::LOAD);
        }
        
        renderPass_ = RHI_CAST(VKRenderPass*, device->GetOrCreateRenderPass(query));
    }
    
    // The create info chains in a bunch of things created on the stack here or inside state
    // objects.
    VkGraphicsPipelineCreateInfo createInfo;
    {
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.stageCount = 2;
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
    
    if (vkCreateGraphicsPipelines(device->GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr,
                                  &vkGraphicsPipeline_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    
}

VKRenderPipeline::~VKRenderPipeline()
{
    // TODO release vulkan resource
    RHI_SAFE_RELEASE(renderPass_);
}

NS_RHI_END
