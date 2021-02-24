#include "GraphicPipeline.h"
#include "Engine/Engine.h"

#include "Backend/Vulkan/VKDevice.h"
#include "Backend/Vulkan/VKSwapChain.h"

NS_RX_BEGIN

GraphicPipeline::GraphicPipeline()
{
	graphicQueue_ = Engine::GetGPUDevice()->GetQueue();
	rhiCommandEncoder_ = Engine::GetGPUDevice()->CreateCommandEncoder();

	RHI::SwapChainDescriptor swapChainDescriptor;
	swapChainDescriptor.device = Engine::GetGPUDevice();
	swapChainDescriptor.format = RHI::TextureFormat::BGRA8UNORM;

	rhiSwapChain_ = new RHI::VKSwapChain((RHI::VKDevice*)Engine::GetGPUDevice(), RHI::GetVkFormat(swapChainDescriptor.format));
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
		rhiDSTextureView_ = rhiDSTexture_->CreateView();
	}
}

void GraphicPipeline::Execute()
{
	RHI::TextureView* colorAttachment = rhiSwapChain_->GetCurrentTexture();

	// init pass
	{
		igniterPass_.Reset();
		igniterPass_.SetupDepthStencilAttachemnt(rhiDSTextureView_);
		igniterPass_.SetupOutputAttachment(0, colorAttachment);
		igniterPass_.Execute(rhiCommandEncoder_);
	}
	
	// draw opaque objects.
	{
		opaquePass_.SetupDepthStencilAttachemnt(rhiDSTextureView_);
		opaquePass_.SetupOutputAttachment(0, colorAttachment);
		opaquePass_.Execute(rhiCommandEncoder_);
	}

	// submit to gpu and present 
	{
		graphicQueue_->Submit(rhiCommandEncoder_->Finish());
		rhiSwapChain_->Present(graphicQueue_);
	}
}

NS_RX_END