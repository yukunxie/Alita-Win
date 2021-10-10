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
		return Material_;
	}

	~MeshComponent();

	Geometry* GetGeometry() const
	{
		return Geometry_;
	}

	virtual void Tick(float dt) override;

	RenderObject* GetRenderObject() { return &RenderObject_; }

	void SetupRenderObject();

	std::uint64_t GetRenderSet()
	{
		return RenderSetBits_;
	}

	void SetRenderSet(std::uint64_t renderSet)
	{
		RenderSetBits_ = renderSet;
	}

	void UnionRenderSet(std::uint64_t renderSet)
	{
		RenderSetBits_ |= renderSet;
	}

	void RemoveRenderSet(std::uint64_t renderSet)
	{
		RenderSetBits_ &= ~renderSet;
	}

	void SetSelected(bool selected);

	bool IsSelected() const { return IsSelected_; }

	void SetPhysicsData(void* data) { PhysicsData_ = data; }

public:
	RenderObject	RenderObject_;
	Material*		Material_	= nullptr;
	Geometry*		Geometry_	= nullptr;
	void*			PhysicsData_ = nullptr;
	bool			IsSelected_ = false;

	friend class MeshComponentBuilder;
	friend class Terrain;

private:
	std::uint64_t RenderSetBits_ = ERenderSet_Opaque;
};

class MeshComponentBuilder
{
public:
	static MeshComponent* CreateBox(const std::string& material = "");
};

NS_RX_END

#endif //ALITA_MESHCOMPONENT_H
