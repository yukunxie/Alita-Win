//
// Created by realxie on 2019-10-29.
//

#include "Model.h"
#include "MeshComponent.h"
#include "Engine/Engine.h"
#include "Graphics/RenderScene.h"

NS_RX_BEGIN

Model::Model()
{
    //AddComponment(MeshComponentBuilder::CreateBox());
}

Model::~Model()
{
}

void Model::Tick(float dt)
{
    Entity::Tick(dt);
}

NS_RX_END