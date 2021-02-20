//
// Created by realxie on 2019-10-25.
//

#include "World.h"
#include "glm/glm.hpp"

NS_RX_BEGIN

World::World()
{
	SetupDefaultCamera();

	auto level = new Level();

	AddLevel(level);

	level->AddEntity(new Model());
}

void World::SetupDefaultCamera()
{
	if (defaultCamera_)
	{
		return;
	}
	defaultCamera_ = Camera::CreatePerspectiveCamera(45.0f,  800.0f / 1280.0f, 1.f, 1000.0f);
}

void World::Update(float dt)
{
	auto a = defaultCamera_->GetViewMatrix();
	auto b = defaultCamera_->GetProjectMatrix();
	auto c = defaultCamera_->GetMVP();
}



NS_RX_END