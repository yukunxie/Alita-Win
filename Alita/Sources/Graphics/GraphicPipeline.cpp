#include "GraphicPipeline.h"
#include "Engine/Engine.h"
#include "World/MeshComponent.h"

#include "Backend/Vulkan/VKDevice.h"
#include "Backend/Vulkan/VKSwapChain.h"
#include "Backend/Vulkan/VKCommandBuffer.h"

#include "PxPhysicsAPI.h"

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
	CommandEncoder_->Reset();

	ShadowGenPass_.Reset();
	ShadowGenPass_.Execute(renderObjects);

	{
		DeferredPass_.Reset(); 
		DeferredPass_.Setup(&ShadowGenPass_);
		DeferredPass_.Execute(renderObjects);
	}

	/*{
		SkyBoxPass_.Reset();
		SkyBoxPass_.Setup(&DeferredPass_, &DeferredPass_);
		SkyBoxPass_.Execute(renderObjects);
	}

	{
		DownSamplePass_.Reset();
		DownSamplePass_.Setup(&SkyBoxPass_);
		DownSamplePass_.Execute();
	}*/

	{
		ToneMappingPass_.Reset();
		ToneMappingPass_.Setup(&DeferredPass_);
		ToneMappingPass_.Execute();
	}
	
	{
		ScreenResolvePass_.Reset();
		ScreenResolvePass_.Setup(&ToneMappingPass_);
		//ScreenResolvePass_.Setup(&DownSamplePass_);
		ScreenResolvePass_.Execute();
	}

		RHI::CommandBuffer* cmdBuffer = CommandEncoder_->Finish();
		RHI_SAFE_RETAIN(cmdBuffer);

		GraphicQueue_->Submit(1, &cmdBuffer);
		Engine::GetGPUDevice()->OnFrameEnd();
}

NS_RX_END