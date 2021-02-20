//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_MATERIAL_H
#define ALITA_MATERIAL_H

#include "../Base/ObjectBase.h"
#include "Effect.h"

#include <list>

NS_RX_BEGIN

class Material : public ObjectBase
{
protected:
    Effect* effect_ = nullptr;
};

NS_RX_END


#endif //ALITA_MATERIAL_H
