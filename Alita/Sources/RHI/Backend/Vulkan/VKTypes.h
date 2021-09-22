//
// Created by realxie on 2019-10-04.
//

#ifndef RHI_VKTYPES_H
#define RHI_VKTYPES_H

#include "Macros.h"
#include "RHI/Flags.h"
#include "RHI/Descriptors.h"

#include <vulkan/vulkan.h>

NS_RHI_BEGIN

template<typename _Tp>
bool IsZero(const _Tp &value)
{
    return abs(value - 0) < 1e-5;
}

const char* GetVkResultString(VkResult code);

VkFrontFace ToVulkanType(const FrontFace &value);

VkCullModeFlagBits ToVulkanType(const CullMode &value);

VkFormat ToVulkanType(Format format);

VkSampleCountFlagBits ToVulkanType(SampleCountFlagBits bit);

VkAttachmentLoadOp ToVulkanType(AttachmentLoadOp op);

VkAttachmentStoreOp ToVulkanType(AttachmentStoreOp op);

VkImageLayout ToVulkanType(ImageLayout layout);

VkShaderStageFlagBits ToVulkanType(ShaderStageFlagBits bits);

VkVertexInputRate ToVulkanType(VertexInputRate inputRate);

VkSharingMode ToVulkanType(SharingMode sharingMode);

VkImageTiling ToVulkanType(ImageTiling imageTiling);

VkImageType ToVulkanType(ImageType imageType);

VkDescriptorType ToVulkanType(BindingType type);

VkPrimitiveTopology ToVulkanType(PrimitiveTopology topology);

VkVertexInputRate ToVulkanType(InputStepMode mode);

VkFormat ToVulkanType(VertexFormat format);

VkFormat ToVulkanType(TextureFormat format);

std::uint32_t GetTextureFormatPixelSize(TextureFormat format);

TextureFormat ToVulkanType(VkFormat format);

VkBool32 ToVulkanType(bool b);

VkCompareOp ToVulkanType(CompareFunction compareFunction);

VkAttachmentLoadOp ToVulkanType(LoadOp op);

VkAttachmentStoreOp ToVulkanType(StoreOp op);

VkImageType ToVulkanType(TextureDimension dim);

VkFilter ToVulkanType(FilterMode mode);

VkSamplerAddressMode ToVulkanType(AddressMode mode);

VkBlendFactor ToVulkanType(BlendFactor factor);

VkBlendOp ToVulkanType(BlendOp op);

VkImageAspectFlags ToVulkanType(TextureAspect aspect, VkFormat format);

VkImageViewType ToVulkanType(TextureViewDimension dim);

VkStencilOp ToVulkanType(StencilOperation op);

VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags imageUsageFlags);

std::uint32_t GetVertexFormatSize(VertexFormat format);

VkShaderStageFlags GetVkShaderStageFlags(ShaderStageFlags flag);

VkImageUsageFlags
GetVkImageUsageFlags(TextureUsageFlags flags, TextureFormat format = TextureFormat::RGBA8UNORM);

VkBufferUsageFlags GetVkBufferUsageFlags(BufferUsageFlags flags);

VkSampleCountFlagBits GetVkSampleCountFlagBits(std::uint32_t sampleCount);

VkSamplerMipmapMode GetVkSamplerMipmapMode(FilterMode mode);

VkAccessFlags VulkanAccessFlags(TextureUsageFlags usage, TextureFormat format);

VkImageLayout GetVulkanImageLayout(TextureUsageFlags usage, TextureFormat format);

VkPipelineStageFlags VulkanPipelineStage(TextureUsageFlags usage, TextureFormat format);

VkImageAspectFlags VulkanAspectMask(TextureFormat format);

NS_RHI_END

#endif //RHI_VKTYPES_H
