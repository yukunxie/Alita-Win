//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_VERTEXBUFFER_H
#define ALITA_VERTEXBUFFER_H

#include "../Base/ObjectBase.h"

#include <cstdint>

NS_RX_BEGIN

enum VertexBufferAttriKind
{
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
    std::uint32_t mask = 0;
    std::vector<std::uint8_t> buffer;
};


NS_RX_END

#endif //ALITA_VERTEXBUFFER_H
