//
// Created by realxie on 2019-10-29.
//

#include "Entity.h"
#include "Component.h"

NS_RX_BEGIN

Entity::Entity()
    : worldMatrix_(1.0f)
{
}

Entity::~Entity()
{
}

void Entity::AddComponment(Component* componment)
{
    components_.emplace_back(componment);
    componment->SetEntity(this);
}

void Entity::AddChild(Entity* child)
{
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end())
        return;
    children_.push_back(child);
    child->parent_ = this;
}

void Entity::RemoveChild(Entity* child)
{
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end())
    {
        children_.erase(it);
        child->parent_ = nullptr;
    }
}

void Entity::SetParent(Entity* parent)
{
    if (parent_ && parent_ != parent)
    {
        parent_->RemoveChild(this);
    }

    if (parent)
        parent->AddChild(this);
    else
        parent_ = nullptr;
}

void Entity::UpdateWorldMatrix() const
{
    if (!isTransformDirty_)
        return;

    isTransformDirty_ = false;
    
    worldMatrix_ = glm::mat4(1);

    worldMatrix_ = glm::rotate(worldMatrix_, glm::radians(transform_.Rotation().x), TVector3(1.0f, 0.0f, 0.0f));
    worldMatrix_ = glm::rotate(worldMatrix_, glm::radians(transform_.Rotation().y), TVector3(0.0f, 1.0f, 0.0f));
    worldMatrix_ = glm::rotate(worldMatrix_, glm::radians(transform_.Rotation().z), TVector3(0.0f, 0.0f, 1.0f));

    worldMatrix_ = glm::scale(worldMatrix_, transform_.Scale());

    //translate
    worldMatrix_ = glm::translate(worldMatrix_, transform_.Position());
}

void Entity::Tick(float dt)
{
    for (auto& cm : components_)
    {
        cm->Tick(dt);
    }

    for (auto child : children_)
    {
        child->Tick(dt);
    }
}

NS_RX_END