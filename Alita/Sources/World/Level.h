//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_LEVEL_H
#define ALITA_LEVEL_H

#include "../Base/ObjectBase.h"
#include "SceneComponent.h"
#include "Model.h"

#include <list>

NS_RX_BEGIN

class World;

class Level : public Entity
{
public:
    Level(World* world)
        : owner_(world)
    {
    }

    World* GetOwner()
    {
        return owner_;
    }

    void AddEntity(Entity* entity);

    void Tick(float dt);

protected:
    std::vector<Entity*> entities_;
    World* owner_ = nullptr;
};

NS_RX_END


#endif //ALITA_LEVEL_H
