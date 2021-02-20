//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_SUBMESH_H
#define ALITA_SUBMESH_H

#include "Geometry.h"
#include "Graphics/Material.h"

NS_RX_BEGIN

class SubMesh
{
private:
    Material* material_ = nullptr;
    Geometry* geometry_ = nullptr;
};

NS_RX_END


#endif //ALITA_SUBMESH_H
