#include "ImageLoader.h"
#include "stb/stb_image.h"
#include "Base/FileSystem.h"

#include "Backend/Vulkan/VKCommandEncoder.h"
#include "Backend/Vulkan/VKCommandBuffer.h"
#include "Backend/Vulkan/VKTexture.h"

NS_RX_BEGIN

namespace ImageLoader
{

	RHI::Texture* LoadTextureFromUri(const std::string& filename)
	{
		int texWidth, texHeight, texChannels;
		const TData& imageData = FileSystem::GetInstance()->GetBinaryData("Textures/spiderman.jpg");
		stbi_uc* pixels = stbi_load_from_memory(imageData.data(), imageData.size(), &texWidth,
			&texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;
		if (!pixels)
		{
			throw std::runtime_error("failed to load texture image!");
		}

		RHI::ImageCreateInfo imageCreateInfo{
			.imageType = RHI::ImageType::IMAGE_TYPE_2D,
			.format = RHI::Format::R8G8B8A8_UNORM,
			.extent = {(std::uint32_t)texWidth,(std::uint32_t)texHeight, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = RHI::SampleCountFlagBits::SAMPLE_COUNT_1_BIT,
			.tiling = RHI::ImageTiling::LINEAR,
			.sharingMode = RHI::SharingMode::EXCLUSIVE,
			.imageData = pixels,
		};


		RHI::TextureDescriptor descriptor;
		{
			descriptor.sampleCount = 1;
			descriptor.format = RHI::TextureFormat::RGBA8UNORM;
			descriptor.usage = RHI::TextureUsage::SAMPLED | RHI::TextureUsage::COPY_DST;
			descriptor.size = { (std::uint32_t)texWidth, (std::uint32_t)texHeight, 1 };
			descriptor.arrayLayerCount = 1;
			descriptor.mipLevelCount = 1;
			descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		};
		auto texture = Engine::GetGPUDevice()->CreateTexture(descriptor);

		RHI::BufferDescriptor bufferDescriptor;
		{
			bufferDescriptor.size = (std::uint32_t)texWidth * texHeight * 4;
			bufferDescriptor.usage = RHI::BufferUsage::COPY_DST | RHI::BufferUsage::COPY_SRC;
		}

		auto buffer = Engine::GetGPUDevice()->CreateBuffer(bufferDescriptor);
		std::uint8_t* pData = (std::uint8_t*)buffer->MapWriteAsync();
		memcpy(pData, pixels, texWidth * texHeight * 4);
		buffer->Unmap();

		RHI::BufferCopyView bufferCopyView;
		{
			bufferCopyView.buffer = buffer;
			bufferCopyView.offset = 0;
			bufferCopyView.imageHeight = 0;
			bufferCopyView.rowPitch = texWidth * 4;
		}

		RHI::TextureCopyView textureCopyView;
		{
			textureCopyView.texture = texture;
			textureCopyView.origin = { 0, 0, 0 };
			textureCopyView.arrayLayer = 1;
			textureCopyView.mipLevel = 0;
		}

		auto commandEncoder = Engine::GetGPUDevice()->CreateCommandEncoder();


		VkCommandBuffer cmdBuffer = RHI_CAST(RHI::VKCommandEncoder*, commandEncoder)->GetCommandBuffer()->GetNative();

		VkCommandBufferBeginInfo cmdBufferBeginInfo{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				.pNext = nullptr,
				.flags = 0,
				.pInheritanceInfo = nullptr
		};
		vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);

		// TODO realxie
		{
			VkImageMemoryBarrier prePresentBarrier = {};
			prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			prePresentBarrier.pNext = NULL;
			prePresentBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
			prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
			prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			prePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			prePresentBarrier.subresourceRange.baseMipLevel = 0;
			prePresentBarrier.subresourceRange.levelCount = 1;
			prePresentBarrier.subresourceRange.baseArrayLayer = 0;
			prePresentBarrier.subresourceRange.layerCount = 1;
			prePresentBarrier.image = RHI_CAST(RHI::VKTexture*, texture)->GetNative();

			vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, NULL, 0,
				NULL, 1, &prePresentBarrier);
		}


		RHI::Extent3D extent3D = { (std::uint32_t)texWidth, (std::uint32_t)texHeight, 1 };
		commandEncoder->CopyBufferToTexture(bufferCopyView, textureCopyView, extent3D);

		// TODO realxie
		{

			VkImageMemoryBarrier prePresentBarrier = {};
			prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			prePresentBarrier.pNext = NULL;
			prePresentBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
			prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			prePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			prePresentBarrier.subresourceRange.baseMipLevel = 0;
			prePresentBarrier.subresourceRange.levelCount = 1;
			prePresentBarrier.subresourceRange.baseArrayLayer = 0;
			prePresentBarrier.subresourceRange.layerCount = 1;
			prePresentBarrier.image = RHI_CAST(RHI::VKTexture*, texture)->GetNative();

			vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, NULL, 0,
				NULL, 1, &prePresentBarrier);

		}

		Engine::GetGPUDevice()->GetQueue()->Submit(commandEncoder->Finish());
		RHI_SAFE_RELEASE(commandEncoder);

		return texture;

	}

}

NS_RX_END