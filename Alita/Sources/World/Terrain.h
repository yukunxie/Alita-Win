#pragma once
#include "Base/Entity.h"

NS_RX_BEGIN

class Terrain : public Entity
{
public:
    Terrain();
    virtual ~Terrain();

public:
    static Terrain* CreateFromHeightMap(const std::string& imgFilename, float minHeight, float maxHeight, float unit);

public:

    virtual void Tick(float dt) override;
};

NS_RX_END
