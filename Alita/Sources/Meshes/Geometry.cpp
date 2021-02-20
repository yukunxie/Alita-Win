//
// Created by realxie on 2019-10-29.
//

#include "Geometry.h"

#include <memory>

NS_RX_BEGIN

Geometry::Geometry()
{
    std::memset(vertexBuffers_, 0, sizeof(vertexBuffers_[0]) * VertexBufferKindCount);
}

void Geometry::SetVertexBuffer(VertexBufferKind vertexBufferKind, VertexBuffer* buffer)
{
    vertexBuffers_[(int)vertexBufferKind] = buffer;
}

VertexBuffer* Geometry::GetVertexBuffer(VertexBufferKind vertexBufferKind)
{
    return vertexBuffers_[(int)vertexBufferKind];
}

NS_RX_END