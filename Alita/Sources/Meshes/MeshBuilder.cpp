//
// Created by realxie on 2019-10-29.
//

#include "MeshBuilder.h"

NS_RX_BEGIN

Mesh* MeshBuilder::CreateBox()
{
    auto mesh =  new Mesh();
    return mesh;
}

Mesh* MeshBuilder::CreateSphere()
{
    return new Mesh();
}

NS_RX_END