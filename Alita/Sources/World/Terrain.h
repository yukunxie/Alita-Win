#pragma once
#include "Base/Entity.h"

#include "Model.h"

NS_RX_BEGIN

class Terrain : public Model
{
public:
    Terrain();
    virtual ~Terrain();

public:
    static Terrain* CreateFromHeightMap(const std::string& imgFilename, float minHeight, float maxHeight, float unit);
};

NS_RX_END
