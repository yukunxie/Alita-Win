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

	//{
	//	auto terrain = Terrain::CreateFromHeightMap("Textures/heightmap.png", -3.f, .0f, 0.1f, { 5, 5 });
	//	AddEntity(terrain);
	//	auto tex = ImageLoader::LoadTextureFromUri("Textures/grass.jpg");
	//	auto mesh = terrain->GetComponent<MeshComponent>();
	//	mesh->GetMaterial()->SetTexture("tAlbedo", tex);
	//}

	{
		//const auto& models = GLTFLoader::LoadModelFromGLTF("Models/chinesedragon.gltf");
		//const auto& models = GLTFLoader::LoadModelFromGLTF("Models/color_teapot_spheres.gltf");
		//const auto& models = GLTFLoader::LoadModelFromGLTF("Models/deer.gltf");
		//const auto& models = GLTFLoader::LoadModelFromGLTF("Models/cerberus/cerberus.gltf");
		//const auto& models = GLTFLoader::LoadModelFromGLTF("Models/armor/armor.gltf");
		//const auto& models = GLTFLoader::LoadModelFromGLTF("Scenes/Sponza/Sponza.gltf");
		const auto& models = GLTFLoader::LoadModelFromGLTF("Models/DamagedHelmet/glTF/DamagedHelmet.gltf");
		for (auto model : models)
		{
			AddEntity(model);
			//model->SetRotation({ 45, 45, 45 });
		}
	}

	//{
	//	auto model = new Model();
	//	model->AddComponment(MeshComponentBuilder::CreateSphere());
	//	AddEntity(model);
	//	float metallicFactor = 0.1;
	//	model->GetComponent<MeshComponent>()->GetMaterial()->SetFloat("metallicFactor", 0, 1, &metallicFactor);
	//	float roughnessFactor = 0.9;
	//	model->GetComponent<MeshComponent>()->GetMaterial()->SetFloat("roughnessFactor", 0, 1, &roughnessFactor);
	//}

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
	auto wsize = Engine::GetEngine()->GetWindowSize();
	defaultCamera_ = Camera::CreatePerspectiveCamera(45.0, float(wsize.width) / float(wsize.height), 1.0f, 100000.f);
	//defaultCamera_->SetPosition({ 0, 5, 5 });
	//defaultCamera_->SetRotation({ -45, 0, 0 });

	//defaultCamera_->Setup({ 0, 5, 5 }, 0.0f, -45.0f);
	defaultCamera_->LookAt({ 0, -5, 5 }, { 0, 0, 0 }, { 0, 1, 0 });
	/*defaultCamera_->SetPosition({ 0, 15, 15 });
	defaultCamera_->SetRotation({ -45, 0, 0 });*/


	shadowMapCamera_ = new OrthoCamera(-50, 50, 50, -50, 0.01f, 100);
	shadowMapCamera_->LookAt(TVector3( 0, 20.0f, 1), TVector3(0.0f, 0.0f, 0.0f ), TVector3(0.0f, 1.0f, 0.0f));
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