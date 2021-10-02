//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_ENTITY_H
#define ALITA_ENTITY_H

#include "ObjectBase.h"
#include "Component.h"

#include <list>

NS_RX_BEGIN

class Entity : public ObjectBase
{
public:
	Entity();

	virtual ~Entity();

	void AddComponment(Component* componment);

	void AddChild(Entity* child);

	void RemoveChild(Entity* child);

	void SetParent(Entity* parent);

	/*
	 * Get component with RTTI
	 * */
	template <typename Tp_>
	Tp_* GetComponent()
	{
		for (auto comp : components_)
		{
			if (dynamic_cast<Tp_*>(comp))
			{
				return dynamic_cast<Tp_*>(comp);
			}
		}
		return nullptr;
	}

	template<>
	Transform* GetComponent<Transform>()
	{
		return &transform_;
	}

	virtual void Tick(float dt);

	TVector3& GetPosition()
	{
		return transform_.Position();
	}

	void SetPosition(const TVector3& position)
	{
		if (!isTransformDirty_)
		{
			isTransformDirty_ = transform_.Position() == position;
		}
		transform_.Position() = position;
	}

	void SetScale(const TVector3& scale)
	{
		if (!isTransformDirty_)
		{
			isTransformDirty_ = transform_.Scale() == scale;
		}
		transform_.Scale() = scale;
	}

	TVector3& GetScale()
	{
		return transform_.Scale();
	}

	void SetRotation(const TVector3& rotation)
	{
		if (!isTransformDirty_)
		{
			isTransformDirty_ = transform_.Rotation() == rotation;
		}
		transform_.Rotation() = rotation;
	}

	TVector3& GetRotation()
	{
		return transform_.Rotation();
	}

	const TMat4x4& GetWorldMatrix() const
	{
		UpdateWorldMatrix();

		return worldMatrix_;
	}

	void UpdateWorldMatrix() const;

protected:
	Entity* parent_ = nullptr;
	std::vector<Entity*> children_;

	std::list<Component*> components_;

	mutable TMat4x4 worldMatrix_;

	// All entity has a transform attribute
	Transform transform_;

	// boolean fileds
	mutable bool isTransformDirty_ = true;
};

NS_RX_END


#endif //ALITA_ENTITY_H
