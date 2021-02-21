//
// Created by realxie on 2019-10-29.
//

#include "Model.h"
#include "MeshComponent.h"
#include "Engine/Engine.h"

NS_RX_BEGIN

Model::Model()
{
    AddComponment(MeshComponentBuilder::CreateBox());
}

Model::~Model()
{
}

void Model::Tick(float dt)
{
    auto mesh = GetComponent<MeshComponent>();
    Engine::GetEngine()->GetRenderScene()->AddPrimitive(mesh);
}

NS_RX_END