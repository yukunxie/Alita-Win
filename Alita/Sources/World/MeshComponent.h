//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MESHCOMPONENT_H
#define ALITA_MESHCOMPONENT_H

#include "Base/Component.h"
#include "Graphics/Material.h"
#include "Meshes/Geometry.h"
#include "Graphics/RenderObject.h"

NS_RX_BEGIN

class MeshComponentBuilder;
class Terrain;

class MeshComponent : public Component
{
public:
	Material* GetMaterial() const
	{
		return material_;
	}

	Geometry* GetGeometry() const
	{
		return geometry_;
	}

	virtual void Tick(float dt) override;

	RenderObject* GetRenderObject() { return &renderObject_; }

	void SetupRenderObject();

public:
	RenderObject renderObject_;
	Material* material_ = nullptr;
	Geometry* geometry_ = nullptr;

	friend class MeshComponentBuilder;
	friend class Terrain;

private:
	std::uint64_t renderFlags_ = ERenderSet_Opaque;
};

class MeshComponentBuilder
{
public:
	static MeshComponent* CreateBox();
};

NS_RX_END

#endif //ALITA_MESHCOMPONENT_H
