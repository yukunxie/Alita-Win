//
// Created by realxie on 2019-10-10.
//

#include "VKTextureView.h"
#include "VKTypes.h"

NS_RHI_BEGIN

VKTextureView::VKTextureView(VKDevice* device, VKTexture* vkTexture)
{
	vkDevice_ = device->GetDevice();

	texture_ = vkTexture;
	RHI_SAFE_RETAIN(texture_);

	textureSize_ = vkTexture->GetTextureSize();
	textureFormat_ = vkTexture->GetFormat();

	VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	if (vkTexture->GetNativeFormat() == VkFormat::VK_FORMAT_D24_UNORM_S8_UINT)
	{
		aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	VkImageViewCreateInfo viewInfo;
	{
		viewInfo.pNext = nullptr;
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = vkTexture->GetNative();
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = vkTexture->GetNativeFormat();
		viewInfo.components = {
			.r = VK_COMPONENT_SWIZZLE_R,
			.g = VK_COMPONENT_SWIZZLE_G,
			.b = VK_COMPONENT_SWIZZLE_B,
			.a = VK_COMPONENT_SWIZZLE_A,
		};
		viewInfo.subresourceRange = {
			.aspectMask = aspectMask,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};
		viewInfo.flags = 0;
	};
	CALL_VK(vkCreateImageView(vkDevice_, &viewInfo, nullptr, &vkImageView_));
}

VKTextureView::VKTextureView(VKDevice* device, const VkImageViewCreateInfo& imageViewCreateInfo,
	const Extent3D& textureSize)
{
	vkDevice_ = device->GetDevice();
	textureSize_ = textureSize;
	textureFormat_ = GetTextureFormat(imageViewCreateInfo.format);
	CALL_VK(vkCreateImageView(vkDevice_, &imageViewCreateInfo, nullptr, &vkImageView_));
}

VKTextureView::~VKTextureView()
{
	if (vkImageView_)
	{
		vkDestroyImageView(vkDevice_, vkImageView_, nullptr);
	}

	RHI_SAFE_RELEASE(texture_);
}

NS_RHI_END
