//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_SCENECOMPONENT_H
#define ALITA_SCENECOMPONENT_H

#include "../Base/Macros.h"
#include "../Base/ObjectBase.h"
#include "../Base/Component.h"

#include <vector>

NS_RX_BEGIN

class SceneComponent : public Component
{
protected:
    SceneComponent* parent_ = nullptr;
    std::vector<SceneComponent*> children_;
};

NS_RX_END


#endif //ALITA_SCENECOMPONENT_H
