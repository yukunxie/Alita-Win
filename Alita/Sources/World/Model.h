//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MODEL_H
#define ALITA_MODEL_H

#include "../Base/ObjectBase.h"
#include "../Base/Entity.h"
#include "../Meshes/Mesh.h"

NS_RX_BEGIN

class Model : public Entity
{
public:
    Model();
    virtual ~Model();

public:
    
    virtual void Tick(float dt) override;
};

NS_RX_END

#endif //ALITA_MODEL_H
