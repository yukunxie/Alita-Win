//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_VERTEXBUFFER_H
#define ALITA_VERTEXBUFFER_H

#include "../Base/ObjectBase.h"

#include <cstdint>

NS_RX_BEGIN

enum VertexBufferKind
{
    POSITION = 0x1, // xyz
    TEXCOORD = 0x2, // uv 2floats
    DIFFUSE  = 0x4, // rgba 4byte
    NORMAL = 0x8,  // xyz
    TANGENT = 0x10, // xyz
    BINORMAL = 0x20, // xyz
    BITANGENT = 0x30, // xyz
};

constexpr std::uint32_t VertexBufferKindCount = 7;// (std::uint32_t)VertexBufferKind::MAX_COUNT;

class VertexBuffer
{
};


NS_RX_END

#endif //ALITA_VERTEXBUFFER_H
