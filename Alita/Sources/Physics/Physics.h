//
// Created by realxie on 2021-10-09.
//

#pragma once

#include "Base/ObjectBase.h"
#include "Base/Component.h"

#include "PxPhysicsAPI.h"


NS_RX_BEGIN

class Physics
{
public:
	static Physics& GetInstance()
	{
		if (Instance_ == nullptr)
		{
			Instance_ = new Physics();
		}
		return *Instance_;
	}

	void AddPrimitive(const Transform& transform, const std::vector<TVector3>& vertices, const std::vector<uint32>& indices);

	void Tick(float dt);


protected:
	Physics();

protected:
	static Physics* Instance_;
	physx::PxFoundation* pxFoundation_ = nullptr;
	physx::PxPhysics* pxPhysics_ = nullptr;
	physx::PxScene* pxScene_ = nullptr;
	physx::PxCooking* pxCooking_ = nullptr;
	physx::PxMaterial* pxDefaultMaterial_ = nullptr;
};

NS_RX_END
