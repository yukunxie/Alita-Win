//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MESHBUILDER_H
#define ALITA_MESHBUILDER_H

#include "Mesh.h"
#include "Geometry.h"
#include "SubMesh.h"

NS_RX_BEGIN

class MeshBuilder
{
public:
    static Mesh* CreateBox();
    
    static Mesh* CreateSphere();
};

NS_RX_END

#endif //ALITA_MESHBUILDER_H
