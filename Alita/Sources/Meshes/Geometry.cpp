//
// Created by realxie on 2019-10-29.
//

#include "Geometry.h"

#include <memory>

NS_RX_BEGIN

Geometry::Geometry()
{
}

void Geometry::AppendVertexBuffer(VertexBuffer* buffer)
{
    vbStreams_.push_back(buffer);
}

bool Geometry::HasTangent()
{
    for (const auto& vb : vbStreams_)
    {
        if (vb->kind == VertexBufferAttriKind::TANGENT)
            return true;
    }
    return false;
}

bool Geometry::HasBiTangent()
{
    for (const auto& vb : vbStreams_)
    {
        if (vb->kind == VertexBufferAttriKind::BITANGENT)
            return true;
    }
    return false;
}

NS_RX_END