//
// Created by realxie on 2019-11-03.
//

#include "Game.h"
#include "../Meshes/MeshBuilder.h"

GamePlay::GamePlay()
{
    gameEngine_ = NS_RX::Engine::GetEngine();
    
    NS_RX::Model* model = new NS_RX::Model();
    auto mesh = NS_RX::MeshBuilder::CreateBox();
    model->SetMesh(mesh);
    
    gameWorld_ = new NS_RX::World();
    RX_SAFE_RETAIN(gameWorld_);
    gameEngine_->RunWithWorld(gameWorld_);
    
    NS_RX::Level* level = new NS_RX::Level();
    gameWorld_->AddLevel(level);
    level->AddObject(model->GetComponent<NS_RX::SceneComponent>());
}

GamePlay::~GamePlay()
{
}

void GamePlay::InitWorld()
{
}