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

NS_RX_END