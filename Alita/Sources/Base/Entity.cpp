//
// Created by realxie on 2019-10-29.
//

#include "Entity.h"
#include "Component.h"

NS_RX_BEGIN

Entity::Entity()
    : WorldMatrix_(1.0f)
{
}

Entity::~Entity()
{
}

void Entity::AddComponment(Component* componment)
{
    Components_.emplace_back(componment);
    componment->SetEntity(this);
}

void Entity::AddChild(Entity* child)
{
    auto it = std::find(Children_.begin(), Children_.end(), child);
    if (it != Children_.end())
        return;
    Children_.push_back(child);
    child->Parent_ = this;
}

void Entity::RemoveChild(Entity* child)
{
    auto it = std::find(Children_.begin(), Children_.end(), child);
    if (it != Children_.end())
    {
        Children_.erase(it);
        child->Parent_ = nullptr;
    }
}

void Entity::SetParent(Entity* parent)
{
    if (Parent_ && Parent_ != parent)
    {
        Parent_->RemoveChild(this);
    }

    if (parent)
        parent->AddChild(this);
    else
        Parent_ = nullptr;
}

void Entity::UpdateWorldMatrix() const
{
    if (!IsTransformDirty_)
        return;

    IsTransformDirty_ = false;

    WorldMatrix_ = glm::mat4(1);

    //translate
    WorldMatrix_ = glm::translate(WorldMatrix_, Transform_.Position());

    WorldMatrix_ = glm::scale(WorldMatrix_, Transform_.Scale());

    WorldMatrix_ = glm::rotate(WorldMatrix_, glm::radians(Transform_.Rotation().x), TVector3(1.0f, 0.0f, 0.0f));
    WorldMatrix_ = glm::rotate(WorldMatrix_, glm::radians(Transform_.Rotation().y), TVector3(0.0f, 1.0f, 0.0f));
    WorldMatrix_ = glm::rotate(WorldMatrix_, glm::radians(Transform_.Rotation().z), TVector3(0.0f, 0.0f, 1.0f));
}

void Entity::Tick(float dt)
{
    for (auto& cm : Components_)
    {
        cm->Tick(dt);
    }

    for (auto child : Children_)
    {
        child->Tick(dt);
    }
}

NS_RX_END