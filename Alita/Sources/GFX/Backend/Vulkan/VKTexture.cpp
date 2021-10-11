//
// Created by realxie on 2019-10-07.
//

#include "VKTexture.h"
#include "VKTypes.h"
#include "VKTextureView.h"
#include "VKTextureViewManager.h"
#include "VKQueue.h"
#include "DeferredRenderCommands.h"
#include "VKCommandBuffer.h"

NS_GFX_BEGIN

extern PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectName;

VKTexture::VKTexture(VKDevice* device)
    : Texture(device)
{

}

void VKTexture::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    DisposeNativeHandle();
    
    GFX_DISPOSE_END();
}

VKTexture::~VKTexture()
{
    Dispose();
}

bool VKTexture::Init(const TextureDescriptor &descriptor)
{
    Texture::Init(descriptor);
    auto device = VKDEVICE();
    vkDevice_ = device->GetNative();
    vkFormat_ = ToVulkanType(textureFormat_);
    textureUsage_ = descriptor.usage;
    
    std::uint32_t queueFamilyIndex = device->GetGraphicQueueFamilyIndex();
    
    VkImageCreateInfo imageInfo;
    {
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.flags = arrayLayerCount_ >= 6 ? (std::uint32_t)VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
        imageInfo.pNext = nullptr;
        imageInfo.format = vkFormat_;
        imageInfo.imageType = ToVulkanType(textureDimension_);
        imageInfo.extent = { textureSize_.width, textureSize_.height, textureSize_.depth };
        imageInfo.mipLevels = mipLevelCount_;
        imageInfo.arrayLayers = arrayLayerCount_;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.usage = GetVkImageUsageFlags(descriptor.usage, textureFormat_);
        imageInfo.samples = GetVkSampleCountFlagBits(sampleCount_);
        imageInfo.queueFamilyIndexCount = 1;
        imageInfo.pQueueFamilyIndices = &queueFamilyIndex;
    }

    memoryUsage_ = textureSize_.width * textureSize_.height *  GetTextureFormatPixelSize(textureFormat_);


#if USE_VULKAN_MEMORY_ALLCATOR
    VmaAllocationCreateInfo vmaAllocCreateInfo = {};
    vmaAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vmaAllocCreateInfo.flags = 0;
    vmaAllocCreateInfo.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    
    vmaCreateImage(VKDEVICE()->GetVmaAllocator(), &imageInfo, &vmaAllocCreateInfo, &vkImage_,
                   &vmaAllocation_, &vmaAllocationInfo_);
#else
    CALL_VK(vkCreateImage(vkDevice_, &imageInfo, nullptr, &vkImage_));

    if (!descriptor.debugName.empty() && vkDebugMarkerSetObjectName && VKDEVICE()->SupportDebugGroup())
    {
        VkDebugMarkerObjectNameInfoEXT nameInfo = {};
        {
            nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
            nameInfo.pNext = nullptr;
            nameInfo.objectType = VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT;
            nameInfo.object = (uint64_t)vkImage_;
            nameInfo.pObjectName = descriptor.debugName.c_str();
        }

        vkDebugMarkerSetObjectName(vkDevice_, &nameInfo);
    }
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vkDevice_, vkImage_, &memRequirements);
    
    VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    
    VkMemoryAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits,
                                                  memoryPropertyFlags),
    };
    
    CALL_VK(vkAllocateMemory(vkDevice_, &allocInfo, nullptr, &vkDeviceMemory_));
    CALL_VK(vkBindImageMemory(vkDevice_, vkImage_, vkDeviceMemory_, 0));
#endif
    
    if (textureUsage_ & TextureUsage::PRESENT)
    {
        VKCommandBuffer* commandBuffer = GFX_CAST(VKQueue*, VKDEVICE()->GetQueue())->GetImageLayoutInitCommandBuffer();
        commandBuffer->RecordCommand<DeferredCmdPipelineBarrier>(this, TextureUsage::UNDEFINED, TextureUsage::PRESENT);
        commandBuffer->AddBindingObject(this);
        currentTexUsageForMemLayout_ = TextureUsage::PRESENT;
    }
    
    return vkImage_ != VK_NULL_HANDLE;
}

bool VKTexture::Init(VkImage vkImage, const TextureDescriptor &descriptor)
{
    Texture::Init(descriptor);
    
    isVkImageWrapper_ = true;
    vkImage_ = vkImage;
    
    auto device = VKDEVICE();
    vkDevice_ = device->GetNative();
    vkFormat_ = ToVulkanType(descriptor.format);
    
    if (textureUsage_ & TextureUsage::PRESENT)
    {
        VKCommandBuffer* commandBuffer = GFX_CAST(VKQueue*, VKDEVICE()->GetQueue())->GetImageLayoutInitCommandBuffer();
        commandBuffer->RecordCommand<DeferredCmdPipelineBarrier>(this, TextureUsage::UNDEFINED, TextureUsage::PRESENT);
        commandBuffer->AddBindingObject(this);
        currentTexUsageForMemLayout_ = TextureUsage::PRESENT;
    }
    
    return true;
}

void VKTexture::DisposeNativeHandle()
{
#if !defined(USE_VULKAN_MEMORY_ALLCATOR) || !USE_VULKAN_MEMORY_ALLCATOR
    if (vkDeviceMemory_)
    {
        if (!isVkImageWrapper_)
        {
            vkFreeMemory(vkDevice_, vkDeviceMemory_, nullptr);
        }
        vkDeviceMemory_ = VK_NULL_HANDLE;
    }
#endif
    
    if (vkImage_)
    {
        if (!isVkImageWrapper_)
        {
#if USE_VULKAN_MEMORY_ALLCATOR
            vmaDestroyImage(VKDEVICE()->GetVmaAllocator(), vkImage_, vmaAllocation_);
            vmaAllocation_ = nullptr;
#else
            vkDestroyImage(vkDevice_, vkImage_, nullptr);
#endif
        }
        vkImage_ = VK_NULL_HANDLE;
    }
}

void VKTexture::SetVkImageHandleDirectly(VkImage vkImage)
{
    GFX_ASSERT(vkImage_ == VK_NULL_HANDLE);
    
    vkImage_ = vkImage;
    currentTexUsageForMemLayout_ = TextureUsage::UNDEFINED;
    
    if (textureUsage_ & TextureUsage::PRESENT)
    {
        VKCommandBuffer* commandBuffer = GFX_CAST(VKQueue*, VKDEVICE()->GetQueue())->GetImageLayoutInitCommandBuffer();
        commandBuffer->RecordCommand<DeferredCmdPipelineBarrier>(this, TextureUsage::UNDEFINED, TextureUsage::PRESENT);
        commandBuffer->AddBindingObject(this);
        currentTexUsageForMemLayout_ = TextureUsage::PRESENT;
    }
    
    const auto& tvs = VKDEVICE()->GetTextureViewManager()->GetAllCreatedTextureViews(this);
    for (VKTextureView* tv : tvs)
    {
        tv->Recreate();
    }
}

TextureView* VKTexture::CreateView(const TextureViewDescriptor &descriptor)
{
    return VKDEVICE()->CreateTextureView(this, descriptor);
}

void VKTexture::TransToCopySrcImageLayout(VKCommandBuffer* commandBuffer)
{
    if (currentTexUsageForMemLayout_ == TextureUsage::COPY_SRC)
    {
        return;
    }
    TransToTargetImageLayout_(commandBuffer, TextureUsage::COPY_SRC);
}

void VKTexture::TransToCopyDstImageLayout(VKCommandBuffer* commandBuffer)
{
    if (currentTexUsageForMemLayout_ == TextureUsage::COPY_DST)
    {
        return;
    }
    TransToTargetImageLayout_(commandBuffer, TextureUsage::COPY_DST);
}

void VKTexture::TransToOutputAttachmentImageLayout(VKCommandBuffer* commandBuffer)
{
    TextureUsageFlags targetTextureUsage = IsSwapchainImage() ?  TextureUsage::PRESENT : TextureUsage::OUTPUT_ATTACHMENT;
    
    if (currentTexUsageForMemLayout_ == targetTextureUsage)
    {
        return;
    }
    
    TransToTargetImageLayout_(commandBuffer, targetTextureUsage);
}

void VKTexture::TransToSampledImageLayout(VKCommandBuffer* commandBuffer)
{
    if (currentTexUsageForMemLayout_ == TextureUsage::SAMPLED)
    {
        return;
    }
    TransToTargetImageLayout_(commandBuffer, TextureUsage::SAMPLED);
}

void VKTexture::TransToTargetImageLayout_(VKCommandBuffer* commandBuffer, TextureUsageFlags targetTextureUsage)
{
    // commandBuffer->RecordCommand<DeferredCmdPipelineBarrier>(this, currentImageLayout_, targetTextureUsage);
    VkPipelineStageFlags srcStages = VulkanPipelineStage(currentTexUsageForMemLayout_, textureFormat_);
    VkPipelineStageFlags dstStages = VulkanPipelineStage(targetTextureUsage, textureFormat_);

    TextureFormat format = GetFormat();

    VkImageMemoryBarrier barrier;
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.srcAccessMask = VulkanAccessFlags(currentTexUsageForMemLayout_, format);
    barrier.dstAccessMask = VulkanAccessFlags(targetTextureUsage, format);
    barrier.oldLayout = GetVulkanImageLayout(currentTexUsageForMemLayout_, format);
    barrier.newLayout = GetVulkanImageLayout(targetTextureUsage, format);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = vkImage_;

    // This transitions the whole resource but assumes it is a 2D texture
    GFX_ASSERT(textureDimension_ == TextureDimension::TEXTURE_2D);
    barrier.subresourceRange.aspectMask = VulkanAspectMask(format);
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevelCount_;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = arrayLayerCount_;

    vkCmdPipelineBarrier(commandBuffer->GetNative(), srcStages, dstStages, 0, 0, NULL, 0, NULL, 1, &barrier);
    
    currentTexUsageForMemLayout_ = targetTextureUsage;
}

NS_GFX_END