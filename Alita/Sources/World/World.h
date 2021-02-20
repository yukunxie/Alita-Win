//
// Created by realxie on 2019-10-25.
//

#ifndef ALITA_WORLD_H
#define ALITA_WORLD_H

#include "../Base/Macros.h"
#include "SceneComponent.h"
#include "Level.h"
#include "Camera.h"

NS_RX_BEGIN

class Camera;

class World : public ObjectBase
{
public:
    World();
    
    void Update(float dt);
    
    void AddLevel(Level* level)
    {
        levels_.push_back(level);
    }

private:
    void SetupDefaultCamera();

protected:
    std::vector<Level*> levels_;

    Camera* defaultCamera_ = nullptr;

};

NS_RX_END

#endif //ALITA_WORLD_H
