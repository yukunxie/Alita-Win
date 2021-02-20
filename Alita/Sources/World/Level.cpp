//
// Created by realxie on 2019-10-29.
//

#include "Level.h"

NS_RX_BEGIN

void Level::AddEntity(Entity* entity)
{
	if (std::find(entities_.begin(), entities_.end(), entity) == entities_.end())
	{
		entities_.push_back(entity);
	}
}

NS_RX_END