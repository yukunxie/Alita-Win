//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_EFFECT_H
#define ALITA_EFFECT_H

#include "../Base/ObjectBase.h"
#include "Technique.h"

NS_RX_BEGIN

class Effect : public ObjectBase
{
public:
    std::vector<Technique*> techniques_;
};

NS_RX_END

#endif //ALITA_EFFECT_H
