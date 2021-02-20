//
// Created by realxie on 2019-10-30.
//

#ifndef ALITA_MESH_H
#define ALITA_MESH_H

#include "SubMesh.h"
#include "AbstractMesh.h"

#include <list>

NS_RX_BEGIN

class Mesh : public AbstractMesh
{
public:
    Mesh() = default;
    
protected:
    std::list<SubMesh*> subMeshes_;
};

NS_RX_END


#endif //ALITA_MESH_H
