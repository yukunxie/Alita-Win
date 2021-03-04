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
    static Terrain* CreateFromHeightMap(const std::string& imgFilename, float minHeight, float maxHeight, float unit, const TVector2& texCoordScale = {1, 1});
};

NS_RX_END
