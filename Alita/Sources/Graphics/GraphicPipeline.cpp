#include "GraphicPipeline.h"
#include "Engine/Engine.h"
#include "World/MeshComponent.h"

#include "Backend/Vulkan/VKDevice.h"
#include "Backend/Vulkan/VKSwapChain.h"
#include "Backend/Vulkan/VKCommandBuffer.h"

int g_test_released = false;

NS_RX_BEGIN

GraphicPipeline::GraphicPipeline()
	: ScreenResolvePass_()
{
	LOGI("xxxx 0");

	GraphicQueue_ = Engine::GetGPUDevice()->GetQueue();
	RHI_SAFE_RETAIN(GraphicQueue_);

	LOGI("xxxx 1");

	CommandEncoder_ = Engine::GetGPUDevice()->CreateCommandEncoder();
	RHI_SAFE_RETAIN(CommandEncoder_);
	RHI_SAFE_RETAIN(CommandEncoder_);
	g_test_released = true;

	LOGI("xxxx 2");

	RHI::SwapChainDescriptor swapChainDescriptor;
	swapChainDescriptor.device = Engine::GetGPUDevice();
	swapChainDescriptor.format = RHI::TextureFormat::RGBA8UNORM;

	RHISwapChain_ = Engine::GetGPUDevice()->CreateSwapchain(swapChainDescriptor);
	RHI_SAFE_RETAIN(RHISwapChain_);

	WindowSize_ = { RHISwapChain_->GetExtent().width, RHISwapChain_->GetExtent().height };
	{
		RHI::TextureDescriptor descriptor;
		{
			descriptor.sampleCount = 1;
			descriptor.format = RHI::TextureFormat::DEPTH24PLUS_STENCIL8;
			descriptor.usage = RHI::TextureUsage::OUTPUT_ATTACHMENT;
			descriptor.size = { WindowSize_.width, WindowSize_.height };
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

GraphicPipeline::~GraphicPipeline()
{
	RHI_SAFE_RELEASE(CommandEncoder_);
}

void GraphicPipeline::Execute(const std::vector<RenderObject*>& renderObjects)
{
	RHI::TextureView* colorAttachment = RHISwapChain_->GetCurrentTexture()->CreateView({});
	CommandEncoder_->Reset();

	//// init pass
	//{
	//	IgniterPass_.Reset();
	//	IgniterPass_.SetupDepthStencilAttachemnt(rhiDSTextureView_);
	//	IgniterPass_.SetupOutputAttachment(0, colorAttachment);
	//	IgniterPass_.Execute(CommandEncoder_, renderObjects);
	//}

	ShadowGenPass_.Reset();
	ShadowGenPass_.Execute(CommandEncoder_, renderObjects);
	//
	// draw opaque objects.
	//{
	//	OpaquePass_.Reset();
	//	//OpaquePass_.SetupDepthStencilAttachemnt(rhiDSTextureView_);
	//	//OpaquePass_.SetupOutputAttachment(0, colorAttachment);
	//	OpaquePass_.Execute(CommandEncoder_, renderObjects);
	//}

	{
		DeferredPass_.Reset(); 
		DeferredPass_.Setup(&ShadowGenPass_);
		DeferredPass_.Execute(CommandEncoder_, renderObjects);
	}

	{
		SkyBoxPass_.Reset();
		SkyBoxPass_.Execute(CommandEncoder_, renderObjects);
	}

	{
		ScreenResolvePass_.Reset();
		//ScreenResolvePass_.Setup(&DeferredPass_);
		ScreenResolvePass_.Setup(&SkyBoxPass_);
		ScreenResolvePass_.SetupOutputAttachment(0, colorAttachment);
		ScreenResolvePass_.Execute(CommandEncoder_);
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

		RHI::CommandBuffer* cmdBuffer = CommandEncoder_->Finish();
		RHI_SAFE_RETAIN(cmdBuffer);

	//	vkCmdPipelineBarrier(RHI_CAST(RHI::VKCommandBuffer*, cmdBuffer)->GetNative(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	//		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0,
	//		NULL, 1, &prePresentBarrier);

		GraphicQueue_->Submit(1, &cmdBuffer);
		Engine::GetGPUDevice()->OnFrameEnd();
		//RHISwapChain_->Present(GraphicQueue_, nullptr);
	//}
}

NS_RX_END