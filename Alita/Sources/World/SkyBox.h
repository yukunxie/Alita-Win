#pragma once
#include "Base/Entity.h"

#include "Model.h"

NS_RX_BEGIN

class SkyBox : public Model
{
public:
    SkyBox();
    virtual ~SkyBox();

    virtual void Tick(float dt) override;
};

NS_RX_END
