#pragma once

#include "RHI.h"

#include "Base/Macros.h"
#include "World/MeshComponent.h"
#include "GraphicPipeline.h"

NS_RX_BEGIN

class RenderScene
{
public:
	RenderScene();
	virtual ~RenderScene();

public:
	void AddPrimitive(MeshComponent* mesh);

	void SubmitGPU();

    void testRotate();

protected:

    GraphicPipeline* graphicPipeline_ = nullptr;

	/*RHI::Buffer* rhiVertexBuffer_ = nullptr;
	RHI::Buffer* rhiIndexBuffer_ = nullptr;
	RHI::CommandEncoder* rhiCommandEncoder_ = nullptr;

	RHI::Shader* rhiVertShader_ = nullptr;
	RHI::Shader* rhiFragShader_ = nullptr;*/

    // for testing.
    RHI::CanvasContext* rhiCanvasContext_ = nullptr;
    RHI::Device* rhiDevice_ = nullptr;
    RHI::Buffer* rhiVertexBuffer_ = nullptr;
    RHI::Buffer* rhiIndexBuffer_ = nullptr;
    RHI::Buffer* rhiUniformBuffer_ = nullptr;
    RHI::BindGroup* rhiBindGroup_ = nullptr;
    RHI::BindingResource* rhiBindingBuffer_ = nullptr;
    RHI::BindingResource* rhiBindingCombined_ = nullptr;
    RHI::PipelineLayout* rhiPipelineLayout_ = nullptr;
    RHI::BindGroupLayout* rhiBindGroupLayout_ = nullptr;
    RHI::Texture* rhiTexture_ = nullptr;
    RHI::TextureView* rhiTextureView_ = nullptr;
    RHI::Texture* rhiDSTexture_ = nullptr;
    RHI::TextureView* rhiDSTextureView_ = nullptr;
    RHI::Sampler* rhiSampler_ = nullptr;
    RHI::RenderPipeline* rhiGraphicPipeline_ = nullptr;
    RHI::Shader* rhiVertShader_ = nullptr;
    RHI::Shader* rhiFragShader_ = nullptr;
    RHI::CommandEncoder* rhiCommandEncoder_ = nullptr;
    RHI::SwapChain* rhiSwapChain_ = nullptr;
};

NS_RX_END