//
// Created by realxie on 2019-10-25.
//

#include "World.h"
#include "glm/glm.hpp"

#include "Loaders/EntityLoader.h"
#include "Terrain.h"

NS_RX_BEGIN

World::World()
{
	SetupDefaultCamera();

	/*auto level = new Level(this);

	AddLevel(level);*/

	//auto model = EntityLoader::LoadModelFromGLTF("Models/cube.gltf");
	//if (model)
	//{

	//}
	//else
	{
		AddEntity(new Model());
	}

	{
		auto terrain = Terrain::CreateFromHeightMap("Textures/heightmap.png", -3.f, .0f, 0.05f);
		AddEntity(terrain);
	}
}

void World::AddEntity(Entity* entity)
{
	if (std::find(entities_.begin(), entities_.end(), entity) == entities_.end())
	{
		entities_.push_back(entity);
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
	defaultCamera_->Tick(dt);

	for (auto it : entities_)
	{
		it->Tick(dt);
	}
}



NS_RX_END