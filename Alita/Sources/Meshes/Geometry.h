//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_GEOMETRY_H
#define ALITA_GEOMETRY_H

#include "../Base/ObjectBase.h"
#include "VertexBuffer.h"

NS_RX_BEGIN

class MeshComponentBuilder;

class Geometry
{
public:
    Geometry();
    
    void AppendVertexBuffer(VertexBuffer* buffer);
    
    const std::vector<VertexBuffer*>& GetVBStreams()
    {
        return vbStreams_;
    }
    
protected:
    std::vector<VertexBuffer*> vbStreams_;
    std::vector<std::uint32_t> indexBuffer_;

    friend class MeshComponentBuilder;
};

NS_RX_END


#endif //ALITA_GEOMETRY_H
