//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MESHCOMPONENT_H
#define ALITA_MESHCOMPONENT_H

#include "Base/Component.h"
#include "Graphics/Material.h"
#include "Meshes/Geometry.h"

NS_RX_BEGIN

class MeshComponentBuilder;

class MeshComponent : public Component
{

protected:
	Material* material_ = nullptr;
	Geometry* geometry_ = nullptr;

	friend class MeshComponentBuilder;
};

class MeshComponentBuilder
{
public:
	static MeshComponent* CreateBox();
};

NS_RX_END

#endif //ALITA_MESHCOMPONENT_H
