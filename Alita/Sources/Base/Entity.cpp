//
// Created by realxie on 2019-10-29.
//

#include "Entity.h"
#include "Component.h"

NS_RX_BEGIN

void Entity::AddComponment(Component* componment)
{
    components_.emplace_back(componment);
    componment->SetEntity(this);
}

NS_RX_END