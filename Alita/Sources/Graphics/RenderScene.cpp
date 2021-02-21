#include "RenderScene.h"
#include "Engine/Engine.h"
#include "RHI.h"
#include "Base/FileSystem.h"
#include "Backend/Vulkan/ShaderHelper.h"

NS_RX_BEGIN

struct UniformBufferObject
{
	TMat4x4 model;
	TMat4x4 view;
	TMat4x4 proj;
};

RenderScene::RenderScene()
{
	rhiDevice_ = Engine::GetGPUDevice();

	rhiCommandEncoder_ = Engine::GetEngine()->GetGPUDevice()->CreateCommandEncoder();

	{
		std::string shaderText = FileSystem::GetInstance()->GetStringData("Shaders/shader.vert.gl");

		auto spirV = RHI::CompileGLSLToSPIRV(shaderText, RHI::ShaderType::VERTEX);
		RHI::ShaderModuleDescriptor descriptor;
		descriptor.binaryCode = std::move(spirV);
		descriptor.codeType = RHI::ShaderCodeType::BINARY;
		rhiVertShader_ = Engine::GetGPUDevice()->CreateShaderModule(descriptor);
	}

	{
		std::string shaderText = FileSystem::GetInstance()->GetStringData("Shaders/shader.frag.gl");

		auto spirV = RHI::CompileGLSLToSPIRV(shaderText, RHI::ShaderType::FRAGMENT);
		RHI::ShaderModuleDescriptor descriptor;
		descriptor.binaryCode = std::move(spirV);
		descriptor.codeType = RHI::ShaderCodeType::BINARY;
		rhiFragShader_ = Engine::GetGPUDevice()->CreateShaderModule(descriptor);
	}

	{
		RHI::BindGroupLayoutDescriptor descriptor;
		descriptor.bindings = {
			RHI::BindGroupLayoutBinding{
				.binding = 0,
				.visibility = RHI::ShaderStage::VERTEX,
				.type = RHI::BindingType::UNIFORM_BUFFER,
			}
			/*,
			RHI::BindGroupLayoutBinding{
				.binding = 1,
				.type = RHI::BindingType::SAMPLED_TEXTURE,
				.visibility = RHI::ShaderStage::FRAGMENT,
			},*/
		};
		rhiBindGroupLayout_ = rhiDevice_->CreateBindGroupLayout(descriptor);
	}

	{
		RHI::PipelineLayoutDescriptor descriptor;
		descriptor.bindGroupLayouts = { rhiBindGroupLayout_ };
		rhiPipelineLayout_ = rhiDevice_->CreatePipelineLayout(descriptor);
	}

	{
		RHI::RenderPipelineDescriptor renderPipelineDescriptor;
		{
			renderPipelineDescriptor.layout = rhiPipelineLayout_;

			renderPipelineDescriptor.vertexStage = {
				.shaderModule = rhiVertShader_,
				.entryPoint = "main"
			};

			renderPipelineDescriptor.fragmentStage = {
				.shaderModule = rhiFragShader_,
				.entryPoint = "main"
			};

			renderPipelineDescriptor.primitiveTopology = RHI::PrimitiveTopology::TRIANGLE_LIST;

			renderPipelineDescriptor.depthStencilState = RHI::DepthStencilStateDescriptor{
				.format = RHI::TextureFormat::DEPTH24PLUS_STENCIL8,
				.depthWriteEnabled = true,
				.depthCompare = RHI::CompareFunction::ALWAYS,
				.stencilFront = {},
				.stencilBack = {},
			};

			renderPipelineDescriptor.vertexInput = {
				.indexFormat = RHI::IndexFormat::UINT32,
				.vertexBuffers = {
					RHI::VertexBufferDescriptor{
						.stride = sizeof(TVertex),
						.stepMode = RHI::InputStepMode::VERTEX,
						.attributeSet = {
							RHI::VertexAttributeDescriptor{
								.offset = offsetof(TVertex, pos),
								.format = RHI::VertexFormat::FLOAT3,
								.shaderLocation = 0,
							},
							RHI::VertexAttributeDescriptor{
								.offset = offsetof(TVertex, color),
								.format = RHI::VertexFormat::FLOAT4,
								.shaderLocation = 1,

							},
							RHI::VertexAttributeDescriptor{
								.offset = offsetof(TVertex, texCoord),
								.format = RHI::VertexFormat::FLOAT2,
								.shaderLocation = 2,
							},
						},
					},
				}
			};
			renderPipelineDescriptor.rasterizationState = {
				.frontFace = RHI::FrontFace::COUNTER_CLOCKWISE,
				.cullMode = RHI::CullMode::BACK_BIT,
			};

			renderPipelineDescriptor.colorStates = {
				RHI::ColorStateDescriptor{
					.format = RHI::TextureFormat::BGRA8UNORM,
					.alphaBlend = {},
					.colorBlend = {},
					.writeMask = RHI::ColorWrite::ALL,
				}
			};

			renderPipelineDescriptor.sampleCount = 1;
			renderPipelineDescriptor.sampleMask = 0xFFFFFFFF;
			renderPipelineDescriptor.alphaToCoverageEnabled = false;
		}

		rhiGraphicPipeline_ = rhiDevice_->CreateRenderPipeline(renderPipelineDescriptor);
		
	}

	

	{
		RHI::Extent3D viewport = { 1280, 800, 1 };

		UniformBufferObject ubo = {};
		float time = 0.0f;
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), float(viewport.width) /
			(float)viewport.height, 0.1f,
			10.0f);
		ubo.proj[1][1] *= -1;

		// Create uniform buffer object
		{
			RHI::BufferSize bufferSize = static_cast<std::uint32_t>(sizeof(UniformBufferObject));
			RHI::BufferDescriptor bufferDescriptor = {
				.size = bufferSize,
				.usage = RHI::BufferUsage::UNIFORM,
			};
			rhiUniformBuffer_ = rhiDevice_->CreateBuffer(bufferDescriptor);

			std::uint8_t* pData = (std::uint8_t*)rhiUniformBuffer_->MapWriteAsync();
			memcpy(pData, &ubo, bufferSize);
			rhiUniformBuffer_->Unmap();
		}

		// setup UBO
		{
			auto bufferBinding = new RHI::BufferBinding(rhiUniformBuffer_, 0,
				(std::uint32_t)sizeof(UniformBufferObject));
			//auto combinedST = new RHI::CombinedSamplerImageViewBinding(rhiSampler_, rhiTextureView_);

			RHI::BindGroupDescriptor descriptor;
			descriptor.layout = rhiBindGroupLayout_;

			descriptor.bindings = {
				RHI::BindGroupBinding{
					.binding = 0,
					.resource = bufferBinding
				},
				/*RHI::BindGroupBinding{
					.binding = 1,
					.resource = combinedST
				}*/
			};
			rhiBindGroup_ = rhiDevice_->CreateBindGroup(descriptor);
		}
	}

	
}

RenderScene::~RenderScene()
{

}

void RenderScene::AddPrimitive(MeshComponent* mesh)
{
	if (!mesh)
	{
		return;
	}

	if (!rhiVertexBuffer_)
	{
		// Create VertexBuffer
		{
			// 1M bytes
			RHI::BufferSize vertexBufferSize = 1024 * 1024;

			RHI::BufferDescriptor vertexBufferDescriptor;
			{
				vertexBufferDescriptor.usage = RHI::BufferUsage::VERTEX;
				vertexBufferDescriptor.size = vertexBufferSize;
			}

			rhiVertexBuffer_ = Engine::GetEngine()->GetGPUDevice()->CreateBuffer(vertexBufferDescriptor);
		}

		{
			RHI::BufferSize indexBufferSize = 10 * 1024;
			RHI::BufferDescriptor indexBufferDescriptor;
			{
				indexBufferDescriptor.usage = RHI::BufferUsage::INDEX;
				indexBufferDescriptor.size = indexBufferSize;
			}
			rhiIndexBuffer_ = Engine::GetEngine()->GetGPUDevice()->CreateBuffer(indexBufferDescriptor);

		}
	}

	std::uint8_t* pVertexData = (std::uint8_t*)rhiVertexBuffer_->MapWriteAsync();
	auto geometry = mesh->GetGeometry();
	VertexBuffer* buffer = geometry->GetVBStreams()[0];
	memcpy(pVertexData, buffer->buffer.data(), buffer->buffer.size());
	rhiVertexBuffer_->Unmap();

	std::uint8_t* pIndexData = (std::uint8_t*)rhiIndexBuffer_->MapWriteAsync();
	memcpy(pIndexData, geometry->GetIndexBuffer()->data(), geometry->GetIndexBuffer()->size());
	rhiIndexBuffer_->Unmap();
}

void RenderScene::SubmitGPU()
{
	std::vector<RHI::RenderPassColorAttachmentDescriptor> colorAttachments = {
	 RHI::RenderPassColorAttachmentDescriptor{
		 .attachment = Engine::GetEngine()->GetSwapchain()->GetCurrentTexture(),
		 .resolveTarget = nullptr,
		 .loadValue = {1.0f, 1.0f, 0.0f, 1.0f},
		 .loadOp = RHI::LoadOp::CLEAR,
		 .storeOp = RHI::StoreOp::STORE,
	 }
	};

	RHI::RenderPassDescriptor renderPassDescriptor;
	renderPassDescriptor.colorAttachments = std::move(colorAttachments);
	renderPassDescriptor.depthStencilAttachment = {
		.attachment = nullptr,
		.depthLoadOp = RHI::LoadOp::CLEAR,
		.depthStoreOp = RHI::StoreOp::STORE,
		.depthLoadValue = 1.0f,
		.stencilLoadOp = RHI::LoadOp::CLEAR,
		.stencilStoreOp = RHI::StoreOp::STORE,
		.stencilLoadValue = 0,
	};

	auto renderPassEncoder = rhiCommandEncoder_->BeginRenderPass(renderPassDescriptor);

	// Render a tile with texture.
	{
		renderPassEncoder->SetGraphicPipeline(rhiGraphicPipeline_);
		renderPassEncoder->SetVertexBuffer(rhiVertexBuffer_, 0);
		renderPassEncoder->SetIndexBuffer(rhiIndexBuffer_, 0);
		renderPassEncoder->SetBindGroup(0, rhiBindGroup_);
		const RHI::Extent2D extent = {1280, 800};
		renderPassEncoder->SetViewport(0, 0, extent.width, extent.height, 0, 1);
		renderPassEncoder->SetScissorRect(0, 0, extent.width, extent.height);
		renderPassEncoder->DrawIndxed(36, 0);
	}

	renderPassEncoder->EndPass();

	auto commandBuffer = rhiCommandEncoder_->Finish();

	Engine::GetEngine()->GetGPUDevice()->GetQueue()->Submit(commandBuffer);

	// Render to screen.
	Engine::GetEngine()->GetSwapchain()->Present(Engine::GetEngine()->GetGPUDevice()->GetQueue());
}

NS_RX_END