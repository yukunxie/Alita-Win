#include "GraphicPipeline.h"
#include "Engine/Engine.h"
#include "World/MeshComponent.h"

#include "Backend/Vulkan/VKDevice.h"
#include "Backend/Vulkan/VKSwapChain.h"
#include "Backend/Vulkan/VKCommandBuffer.h"

NS_RX_BEGIN

GraphicPipeline::GraphicPipeline()
	: screenResolvePass_()
{
	LOGI("xxxx 0");

	graphicQueue_ = Engine::GetGPUDevice()->GetQueue();
	RHI_SAFE_RETAIN(graphicQueue_);

	LOGI("xxxx 1");

	rhiCommandEncoder_ = Engine::GetGPUDevice()->CreateCommandEncoder();
	RHI_SAFE_RETAIN(rhiCommandEncoder_);

	LOGI("xxxx 2");

	RHI::SwapChainDescriptor swapChainDescriptor;
	swapChainDescriptor.device = Engine::GetGPUDevice();
	swapChainDescriptor.format = RHI::TextureFormat::RGBA8UNORM;

	rhiSwapChain_ = Engine::GetGPUDevice()->CreateSwapchain(swapChainDescriptor);
	RHI_SAFE_RETAIN(rhiSwapChain_);

	windowSize_ = { rhiSwapChain_->GetExtent().width, rhiSwapChain_->GetExtent().height };
	{
		RHI::TextureDescriptor descriptor;
		{
			descriptor.sampleCount = 1;
			descriptor.format = RHI::TextureFormat::DEPTH24PLUS_STENCIL8;
			descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
			descriptor.size = { windowSize_.width, windowSize_.height };
			descriptor.arrayLayerCount = 1;
			descriptor.mipLevelCount = 1;
			descriptor.dimension = RHI::TextureDimension::TEXTURE_2D;
		};
		rhiDSTexture_ = Engine::GetGPUDevice()->CreateTexture(descriptor);
		RHI_SAFE_RETAIN(rhiDSTexture_);
		rhiDSTextureView_ = rhiDSTexture_->CreateView({});
		RHI_SAFE_RETAIN(rhiDSTextureView_);
	}
}

void GraphicPipeline::Execute(const std::vector<RenderObject*>& renderObjects)
{
	RHI::TextureView* colorAttachment = rhiSwapChain_->GetCurrentTexture()->CreateView({});
	rhiCommandEncoder_->Reset();

	//// init pass
	//{
	//	igniterPass_.Reset();
	//	igniterPass_.SetupDepthStencilAttachemnt(rhiDSTextureView_);
	//	igniterPass_.SetupOutputAttachment(0, colorAttachment);
	//	igniterPass_.Execute(rhiCommandEncoder_, renderObjects);
	//}

	//shadowGenPass_.Reset();
	//shadowGenPass_.Execute(rhiCommandEncoder_, renderObjects);
	//
	// draw opaque objects.
	//{
	//	opaquePass_.Reset();
	//	//opaquePass_.SetupDepthStencilAttachemnt(rhiDSTextureView_);
	//	//opaquePass_.SetupOutputAttachment(0, colorAttachment);
	//	opaquePass_.Execute(rhiCommandEncoder_, renderObjects);
	//}

	{
		deferredPass_.Reset();
		deferredPass_.Execute(rhiCommandEncoder_, renderObjects);
	}

	{
		screenResolvePass_.Reset();
		screenResolvePass_.Setup(&deferredPass_);
		screenResolvePass_.SetupOutputAttachment(0, colorAttachment);
		screenResolvePass_.Execute(rhiCommandEncoder_);
	}

	//// submit to gpu and present 
	//{
	//	VkImageMemoryBarrier prePresentBarrier = {};
	//	prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	//	prePresentBarrier.pNext = NULL;
	//	prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	//	prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	//	prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//	prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	//	prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	prePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//	prePresentBarrier.subresourceRange.baseMipLevel = 0;
	//	prePresentBarrier.subresourceRange.levelCount = 1;
	//	prePresentBarrier.subresourceRange.baseArrayLayer = 0;
	//	prePresentBarrier.subresourceRange.layerCount = 1;
	//	prePresentBarrier.image = RHI_CAST(RHI::VKTextureView*, colorAttachment)->GetVkImage();

		RHI::CommandBuffer* cmdBuffer = rhiCommandEncoder_->Finish();
		RHI_SAFE_RETAIN(cmdBuffer);

	//	vkCmdPipelineBarrier(RHI_CAST(RHI::VKCommandBuffer*, cmdBuffer)->GetNative(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	//		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0,
	//		NULL, 1, &prePresentBarrier);

		graphicQueue_->Submit(1, &cmdBuffer);
		Engine::GetGPUDevice()->OnFrameEnd();
		//rhiSwapChain_->Present(graphicQueue_, nullptr);
	//}
}

NS_RX_END