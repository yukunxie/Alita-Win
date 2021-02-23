//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_VERTEXBUFFER_H
#define ALITA_VERTEXBUFFER_H

#include "Base/ObjectBase.h"

#include "Types/TData.h"

NS_RX_BEGIN

enum VertexBufferAttriKind
{
    INVALID = 0,
    POSITION = 0x1, // xyz
    DIFFUSE  = 0x2, // rgba 4byte
    TEXCOORD = 0x4, // uv 2floats
    NORMAL = 0x8,  // xyz
    TANGENT = 0x10, // xyz
    BINORMAL = 0x20, // xyz
    BITANGENT = 0x30, // xyz
};

constexpr std::uint32_t VertexBufferKindCount = 7;// (std::uint32_t)VertexBufferKind::MAX_COUNT;

struct VertexBuffer
{
    VertexBufferAttriKind kind = VertexBufferAttriKind::INVALID;
    TData buffer;

    void InitData(const void* data, std::uint32_t size)
    {
        buffer.resize(size);
        memcpy(buffer.data(), data, size);
    }
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

    void InitData(const void* data, std::uint32_t size)
    {
        buffer.resize(size);
        memcpy(buffer.data(), data, size);
    }
};


NS_RX_END

#endif //ALITA_VERTEXBUFFER_H
