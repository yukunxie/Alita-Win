//
// Created by realxie on 2019-10-25.
//

#include "World.h"
#include "glm/glm.hpp"

#include "Loaders/EntityLoader.h"

NS_RX_BEGIN

World::World()
{
	SetupDefaultCamera();

	auto level = new Level(this);

	AddLevel(level);

	auto model = EntityLoader::LoadModelFromGLTF("Models/cube.gltf");
	if (model)
	{

	}
	else
	{
		level->AddEntity(new Model());
	}
}

void World::SetupDefaultCamera()
{
	if (defaultCamera_)
	{
		return;
	}
	defaultCamera_ = Camera::CreatePerspectiveCamera(45.0f,  800.0f / 1280.0f, 1.f, 1000.0f);
}

void World::Tick(float dt)
{
	for (auto& level : levels_)
	{
		level->Tick(dt);
	}
}



NS_RX_END