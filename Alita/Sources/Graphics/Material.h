//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MATERIAL_H
#define ALITA_MATERIAL_H

#include "Base/ObjectBase.h"
#include "Base/FileSystem.h"
#include "Effect.h"


#include <list>

NS_RX_BEGIN

class Material : public ObjectBase
{
public:
    Material(const std::string& configFilename = "");
protected:
    Effect* effect_ = nullptr;
};

NS_RX_END


#endif //ALITA_MATERIAL_H
