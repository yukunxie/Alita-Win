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

NS_RX_END