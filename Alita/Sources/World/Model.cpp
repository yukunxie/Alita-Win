//
// Created by realxie on 2019-10-29.
//

#include "Model.h"
#include "MeshComponent.h"

NS_RX_BEGIN

Model::Model()
{
    AddComponment(new MeshComponent());
}

Model::~Model()
{
}

NS_RX_END