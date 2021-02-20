//
// Created by realxie on 2019-10-07.
//

#include "VKTexture.h"
#include "VKTypes.h"
#include "VKTextureView.h"
#include "VKQueue.h"


NS_RHI_BEGIN

//VKTexture::VKTexture(VKDevice* device, const ImageCreateInfo& imageCreateInfo)
//    : device_(device)
//{
//    vkDevice_ = device->GetDevice();
//    std::uint32_t queueFamilyIndex = device->GetGraphicQueueFamilyIndex();
//
//    vkFormat_ = ToVkFormat(imageCreateInfo.format);
//
//    textureFormat_ = GetTextureFormat(vkFormat_);
//
//    // Check for linear supportability
//    VkFormatProperties props;
//    bool needBlit = true;
//    vkGetPhysicalDeviceFormatProperties(device->GetPhysicalDevice(), ToVkFormat(imageCreateInfo.format), &props);
//    assert((props.linearTilingFeatures | props.optimalTilingFeatures) &
//           VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
//
//    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
//        // linear format supporting the required texture
//        needBlit = false;
//    }
//
//    textureSize_ = imageCreateInfo.extent;
//
//    VkImageCreateInfo imageInfo = {
//            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
//            .pNext = nullptr,
//            .format = ToVkFormat(imageCreateInfo.format),
//            .imageType = ToVkImageType(imageCreateInfo.imageType),
//            .extent = {
//                    .width = imageCreateInfo.extent.width,
//                    .height = imageCreateInfo.extent.height,
//                    .depth = imageCreateInfo.extent.depth,
//            },
//            .mipLevels = imageCreateInfo.mipLevels,
//            .arrayLayers = imageCreateInfo.arrayLayers,
//            .tiling = ToVkImageTiling(imageCreateInfo.tiling),
//            .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
//            .sharingMode =  ToVkSharingMode(imageCreateInfo.sharingMode),
//            .usage =(needBlit ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : VK_IMAGE_USAGE_SAMPLED_BIT),
//            .samples = ToVkSampleCountFlagBits(imageCreateInfo.samples),
//            .queueFamilyIndexCount = 1,
//            .pQueueFamilyIndices = &queueFamilyIndex,
//            .flags = 0,
//    };
//
//    VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
//    if (vkFormat_ == VkFormat::VK_FORMAT_D24_UNORM_S8_UINT)
//    {
//        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
//        memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
//    }
//
//    CALL_VK(vkCreateImage(vkDevice_, &imageInfo, nullptr, &vkImage_));
//
//    VkMemoryRequirements memRequirements;
//    vkGetImageMemoryRequirements(vkDevice_, vkImage_, &memRequirements);
//
//    VkMemoryAllocateInfo allocInfo {
//            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
//            .allocationSize = memRequirements.size,
//            .memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags),
//    };
//
//    CALL_VK(vkAllocateMemory(vkDevice_, &allocInfo, nullptr, &vkDeviceMemory_));
//    CALL_VK(vkBindImageMemory(vkDevice_, vkImage_, vkDeviceMemory_, 0));
//
////    RHI_ASSERT(imageCreateInfo.format == Format::R8G8B8A8_UNORM);
//
//    if (imageCreateInfo.imageData)
//    {
//        const VkImageSubresource subres = {
//                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .arrayLayer = 0,
//        };
//        VkSubresourceLayout layout;
//        vkGetImageSubresourceLayout(vkDevice_, vkImage_, &subres,
//                                    &layout);
//
//        VkDeviceSize imageSize = imageCreateInfo.extent.width * imageCreateInfo.extent.height * 4;
//
//        void* data;
//        vkMapMemory(vkDevice_, vkDeviceMemory_, 0, imageSize, 0, &data);
//        memcpy(data, imageCreateInfo.imageData, static_cast<size_t>(imageSize));
//        vkUnmapMemory(vkDevice_, vkDeviceMemory_);
//    }
//
//    SetImageLayout(device);
//}

bool VKTexture::Init(VKDevice* device, const TextureDescriptor &descriptor)
{
    device_ = device;
    vkDevice_ = device->GetDevice();
    vkFormat_ = GetVkFormat(descriptor.format);
    textureFormat_ = descriptor.format;
    
    std::uint32_t queueFamilyIndex = device->GetGraphicQueueFamilyIndex();
    
    textureSize_ = descriptor.size;
    
    VkImageCreateInfo imageInfo; 
    {
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.flags = 0;
        imageInfo.pNext = nullptr;
        imageInfo.format = vkFormat_;
        imageInfo.imageType = GetVkImageType(descriptor.dimension);
        imageInfo.extent = { textureSize_.width, textureSize_.height, textureSize_.depth };
        imageInfo.mipLevels = descriptor.mipLevelCount;
        imageInfo.arrayLayers = descriptor.arrayLayerCount;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.usage = GetVkImageUsageFlags(descriptor.usage, descriptor.format);
        imageInfo.samples = GetVkSampleCountFlagBits(descriptor.sampleCount);
        imageInfo.queueFamilyIndexCount = 1;
        imageInfo.pQueueFamilyIndices = &queueFamilyIndex;
    }
    
    CALL_VK(vkCreateImage(vkDevice_, &imageInfo, nullptr, &vkImage_));
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vkDevice_, vkImage_, &memRequirements);
    
    VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    //    if (vkFormat_ == VkFormat::VK_FORMAT_D24_UNORM_S8_UINT)
    //    {
    //        memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    //    }
    
    VkMemoryAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits,
                                                  memoryPropertyFlags),
    };
    
    CALL_VK(vkAllocateMemory(vkDevice_, &allocInfo, nullptr, &vkDeviceMemory_));
    CALL_VK(vkBindImageMemory(vkDevice_, vkImage_, vkDeviceMemory_, 0));
    
    SetImageLayout(device);
    
    return true;
}

VKTexture::~VKTexture()
{
    if (vkDeviceMemory_)
    {
        vkFreeMemory(vkDevice_, vkDeviceMemory_, nullptr);
    }
    
    if (vkImage_)
    {
        vkDestroyImage(vkDevice_, vkImage_, nullptr);
    }
}


void VKTexture::SetImageLayout(const VKDevice* device)
{
    VkCommandPoolCreateInfo cmdPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = device->GetGraphicQueueFamilyIndex(),
    };
    
    VkCommandPool cmdPool;
    CALL_VK(vkCreateCommandPool(vkDevice_, &cmdPoolCreateInfo, nullptr, &cmdPool));
    
    VkCommandBuffer gfxCmd;
    const VkCommandBufferAllocateInfo cmd = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = cmdPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    
    CALL_VK(vkAllocateCommandBuffers(vkDevice_, &cmd, &gfxCmd));
    VkCommandBufferBeginInfo cmd_buf_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr};
    CALL_VK(vkBeginCommandBuffer(gfxCmd, &cmd_buf_info));
    
    if (vkFormat_ == VkFormat::VK_FORMAT_D24_UNORM_S8_UINT)
    {
        device->SetImageLayout(gfxCmd, vkImage_, VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                               VK_PIPELINE_STAGE_HOST_BIT,
                               VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }
    else
    {
        device->SetImageLayout(gfxCmd, vkImage_, VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                               VK_PIPELINE_STAGE_HOST_BIT,
                               VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }
    
    CALL_VK(vkEndCommandBuffer(gfxCmd));
    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    VkFence fence;
    CALL_VK(vkCreateFence(vkDevice_, &fenceInfo, nullptr, &fence));
    vkResetFences(vkDevice_, 1, &fence);
    
    VkSubmitInfo submitInfo;
    {
        submitInfo.pNext = nullptr;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = nullptr;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &gfxCmd;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;
    }
    const VKQueue* queue = RHI_CAST(const VKQueue*, device->GetQueue());
    CALL_VK(vkQueueSubmit(queue->GetNative(), 1, &submitInfo, fence));
    CALL_VK(vkWaitForFences(vkDevice_, 1, &fence, VK_TRUE, 100000000));
    
    vkDestroyFence(vkDevice_, fence, nullptr);
    vkFreeCommandBuffers(vkDevice_, cmdPool, 1, &gfxCmd);
    vkDestroyCommandPool(vkDevice_, cmdPool, nullptr);
}

TextureView* VKTexture::CreateView()
{
    TextureView* textureView = new VKTextureView(device_, this);
    RHI_SAFE_RETAIN(textureView);
    return textureView;
}

NS_RHI_END