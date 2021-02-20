//
// Created by realxie on 2019-10-29.
//

#include "Component.h"
#include "Entity.h"

NS_RX_BEGIN

void Component::SetEntity(Entity* entity)
{
    owner_ = entity;
}

NS_RX_END