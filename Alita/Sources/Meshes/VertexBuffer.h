//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_VERTEXBUFFER_H
#define ALITA_VERTEXBUFFER_H

#include "Base/ObjectBase.h"
#include "Engine/Engine.h"
#include "Types/TData.h"
#include "RHI.h"

NS_RX_BEGIN

enum VertexBufferAttriKind
{
	INVALID = 0,
	POSITION = 0x1, // xyz
	DIFFUSE = 0x2, // rgba 4byte
	TEXCOORD = 0x4, // uv 2floats
	NORMAL = 0x8,  // xyz
	TANGENT = 0x10, // xyz
	BINORMAL = 0x20, // xyz
	BITANGENT = 0x40, // xyz
	TEXCOORD2 = 0x80, // uv2 2floats
};

constexpr std::uint32_t VertexBufferKindCount = 7;// (std::uint32_t)VertexBufferKind::MAX_COUNT;

struct VertexBuffer
{
	VertexBufferAttriKind kind = VertexBufferAttriKind::INVALID;
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
			gpuBuffer = Engine::GetEngine()->GetGPUDevice()->CreateBuffer(vertexBufferDescriptor);
		}

		gpuBuffer->SetSubData(0, size, data);
	}

public:
	static VertexBufferAttriKind NameToVBAttrKind(const std::string& name);
};

enum IndexType
{
	UINT32,
	UINT16
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
			gpuBuffer = Engine::GetEngine()->GetGPUDevice()->CreateBuffer(vertexBufferDescriptor);
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
