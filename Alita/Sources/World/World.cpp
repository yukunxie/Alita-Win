//
// Created by realxie on 2019-10-25.
//

#include "World.h"
#include "glm/glm.hpp"

#include "Loaders/EntityLoader.h"
#include "Loaders/GLTFLoader.h"
#include "Loaders/ImageLoader.h"
#include "Terrain.h"
#include "SkyBox.h"
#include "MeshComponent.h"

NS_RX_BEGIN

World::World()
{
	SetupDefaultCamera();

	/*{
		AddEntity(new Model());
	}*/

	{
		auto terrain = Terrain::CreateFromHeightMap("Textures/heightmap.png", -3.f, .0f, 0.1f, { 5, 5 });
		AddEntity(terrain);
		auto tex = ImageLoader::LoadTextureFromUri("Textures/grass.jpg");
		auto mesh = terrain->GetComponent<MeshComponent>();
		mesh->GetMaterial()->SetTexture("albedo", tex);
	}

	{
		//const auto& models = GLTFLoader::LoadModelFromGLTF("Models/chinesedragon.gltf");
		const auto& models = GLTFLoader::LoadModelFromGLTF("Models/color_teapot_spheres.gltf");
		//const auto& models = GLTFLoader::LoadModelFromGLTF("Models/deer.gltf");
		//const auto& models = GLTFLoader::LoadModelFromGLTF("Models/armor/armor.gltf");
		for (auto model : models)
		{
			AddEntity(model);
		}
	}

	{
		AddEntity(new SkyBox);
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
	defaultCamera_ = Camera::CreatePerspectiveCamera(45.0, 1280.f / 800.0f, 1.0f, 1000.f);
	defaultCamera_->LookAt({ 0, 15, 15 }, { 0, 0, 0 }, { 0, 1, 0 });

	shadowMapCamera_ = new OrthoCamera(-50, 50, 50, -50, 0.0001f, 1000);
	shadowMapCamera_->LookAt(TVector3( -20.0f, 20.0f, 20.0f), TVector3(0.0f, 0.0f, 0.0f ), TVector3(0.0f, 1.0f, 0.0f));

	//shadowMapCamera_ = Camera::CreatePerspectiveCamera(45.0, 1, 1.0f, 300.f);
	//shadowMapCamera_->LookAt({ 1, 100, 0 }, { 0, 0, 0 }, { 0, 1, 0 });
}

void World::Tick(float dt)
{
	defaultCamera_->Tick(dt);
	shadowMapCamera_->Tick(dt);

	for (auto it : entities_)
	{
		it->Tick(dt);
	}
}

void World::Render()
{
}



NS_RX_END