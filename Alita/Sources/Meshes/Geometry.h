//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_GEOMETRY_H
#define ALITA_GEOMETRY_H

#include "../Base/ObjectBase.h"
#include "VertexBuffer.h"

NS_RX_BEGIN

class Geometry
{
public:
    Geometry();
    
    void SetVertexBuffer(VertexBufferKind vertexBufferKind, VertexBuffer* buffer);
    
    VertexBuffer* GetVertexBuffer(VertexBufferKind vertexBufferKind);
    
protected:
    VertexBuffer* vertexBuffers_[VertexBufferKindCount];
};

NS_RX_END


#endif //ALITA_GEOMETRY_H
