//
// Created by realxie on 2019-10-04.
//

#ifndef ALITA_VKTYPES_H
#define ALITA_VKTYPES_H

#include "Macros.h"
#include "../../Include/Flags.h"
#include "../../Include/Descriptors.h"

#include <vulkan/vulkan.h>

NS_RHI_BEGIN

VkFormat ToVkFormat(Format format);

VkSampleCountFlagBits ToVkSampleCountFlagBits(SampleCountFlagBits bit);

VkAttachmentLoadOp ToVkAttachmentLoadOp(AttachmentLoadOp op);

VkAttachmentStoreOp ToVkAttachmentStoreOp(AttachmentStoreOp op);

VkImageLayout ToVkImageLayout(ImageLayout layout);

VkShaderStageFlagBits ToVkShaderStageFlagBits(ShaderStageFlagBits bits);

VkVertexInputRate ToVkVertexInputRate(VertexInputRate inputRate);

VkSharingMode ToVkSharingMode(SharingMode sharingMode);

VkImageTiling ToVkImageTiling(ImageTiling imageTiling);

VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags imageUsageFlags);

VkImageType ToVkImageType(ImageType imageType);

VkDescriptorType GetVkDescriptorType(BindingType type);

VkShaderStageFlags GetVkShaderStageFlags(ShaderStageFlags flag);

VkPrimitiveTopology GetPrimitiveTopology(PrimitiveTopology topology);

VkVertexInputRate GetVertexInputRate(InputStepMode mode);

std::uint32_t GetVertexFormatSize(VertexFormat format);

VkFormat GetVkFormat(VertexFormat format);

VkFormat GetVkFormat(TextureFormat format);

std::uint32_t GetTextureFormatPixelSize(TextureFormat format);

TextureFormat GetTextureFormat(VkFormat format);

VkBool32 GetVkBoolean(bool b);

VkCompareOp GetCompareOp(CompareFunction compareFunction);

VkAttachmentLoadOp GetLoadOp(LoadOp op);

VkBufferUsageFlags GetVkBufferUsageFlags(BufferUsageFlags flags);

VkImageType GetVkImageType(TextureDimension dim);

VkImageUsageFlags
GetVkImageUsageFlags(TextureUsageFlags flags, TextureFormat format = TextureFormat::RGBA8UNORM);

VkSampleCountFlagBits GetVkSampleCountFlagBits(std::uint32_t sampleCount);

VkFilter GetVkFilter(FilterMode mode);

VkSamplerMipmapMode GetVkSamplerMipmapMode(FilterMode mode);

VkSamplerAddressMode GetVkSamplerAddressMode(AddressMode mode);

NS_RHI_END

#endif //ALITA_VKTYPES_H
