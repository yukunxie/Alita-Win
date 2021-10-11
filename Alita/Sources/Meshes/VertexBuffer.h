//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_VERTEXBUFFER_H
#define ALITA_VERTEXBUFFER_H

#include "Base/ObjectBase.h"
#include "Engine/Engine.h"
#include "Types/TData.h"
#include "GFX/GFX.h"
#include "Graphics/Material.h"

NS_RX_BEGIN

struct VertexBuffer
{
	VertexBufferAttriKind kind = VertexBufferAttriKind::INVALID;
	InputAttributeFormat format;
	std::uint32_t byteStride = 0;

	TData buffer;
	RHI::Buffer* gpuBuffer = nullptr;

	void InitData(const void* data, std::uint32_t size)
	{
		if (buffer.size() >= size && gpuBuffer)
		{
			memcpy(buffer.data(), data, size);
		}
		else
		{
			buffer.resize(size);
			memcpy(buffer.data(), data, size);
			RHI_SAFE_RELEASE(gpuBuffer);
			RHI::BufferDescriptor vertexBufferDescriptor;
			{
				vertexBufferDescriptor.usage = RHI::BufferUsage::VERTEX;
				vertexBufferDescriptor.size = size;
			}
			RHI_PTR_ASSIGN(gpuBuffer, Engine::GetEngine()->GetGPUDevice()->CreateBuffer(vertexBufferDescriptor));
		}

		gpuBuffer->SetSubData(0, size, data);
	}

public:
	static VertexBufferAttriKind NameToVBAttrKind(const std::string& name);
};

struct IndexBuffer
{
	IndexType indexType = IndexType::UINT32;
	TData buffer;
	RHI::Buffer* gpuBuffer = nullptr;

	void InitData(const void* data, std::uint32_t size)
	{
		if (buffer.size() >= size && gpuBuffer)
		{
			memcpy(buffer.data(), data, size);
		}
		else
		{
			buffer.resize(size);
			memcpy(buffer.data(), data, size);
			RHI_SAFE_RELEASE(gpuBuffer);
			RHI::BufferDescriptor vertexBufferDescriptor;
			{
				vertexBufferDescriptor.usage = RHI::BufferUsage::INDEX;
				vertexBufferDescriptor.size = size;
			}
			RHI_PTR_ASSIGN(gpuBuffer, Engine::GetEngine()->GetGPUDevice()->CreateBuffer(vertexBufferDescriptor));
		}

		gpuBuffer->SetSubData(0, size, data);
	}

	std::uint32_t GetIndexCount()
	{
		return buffer.size() / (indexType == IndexType::UINT32 ? 4 : 2);
	}
};


NS_RX_END

#endif //ALITA_VERTEXBUFFER_H
