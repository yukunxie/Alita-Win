//
// Created by realxie on 2019-10-25.
//

#include "World.h"
#include "glm/glm.hpp"

#include "Loaders/EntityLoader.h"
#include "Loaders/GLTFLoader.h"
#include "Loaders/ImageLoader.h"
#include "Terrain.h"
#include "MeshComponent.h"

NS_RX_BEGIN

World::World()
{
	SetupDefaultCamera();

	/*{
		AddEntity(new Model());
	}*/

	//{
	//	auto terrain = Terrain::CreateFromHeightMap("Textures/heightmap.png", -3.f, .0f, 0.05f, { 5, 5 });
	//	AddEntity(terrain);
	//	auto tex = ImageLoader::LoadTextureFromUri("Textures/grass.jpg");
	//	auto mesh = terrain->GetComponent<MeshComponent>();
	//	mesh->GetMaterial()->SetTexture("albedo", tex);
	//}

	{
		const std::vector<Model*>& models = GLTFLoader::LoadModelFromGLTF("Models/chinesedragon.gltf");
		//const std::vector<Model*>& models = GLTFLoader::LoadModelFromGLTF("Models/color_teapot_spheres.gltf");
		for (Model* model : models)
		{
			AddEntity(model);
		}
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

void World::Render()
{
	
}



NS_RX_END