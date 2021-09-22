//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_GEOMETRY_H
#define ALITA_GEOMETRY_H

#include "../Base/ObjectBase.h"
#include "VertexBuffer.h"
#include "RHI/RHI.h"

NS_RX_BEGIN

class MeshComponentBuilder;
class Terrain;

class Geometry
{
public:
    Geometry();
    
    void AppendVertexBuffer(VertexBuffer* buffer);
    
    const std::vector<VertexBuffer*>& GetVBStreams()
    {
        return vbStreams_;
    }

    IndexBuffer* GetIndexBuffer()
    {
        return &indexBuffer_;
    }
    
protected:
    std::vector<VertexBuffer*> vbStreams_;
    IndexBuffer indexBuffer_;

    friend class MeshComponentBuilder;
    friend class Terrain;
};

NS_RX_END


#endif //ALITA_GEOMETRY_H
