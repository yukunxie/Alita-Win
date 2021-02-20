//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_PASS_H
#define ALITA_PASS_H

#include "../Base/ObjectBase.h"

NS_RX_BEGIN

enum class PassType
{
    OPAQUE,
    TRANSPARENT,
    UI,
};

class Pass : public ObjectBase
{
protected:
    PassType passType_ = PassType::OPAQUE;
};

NS_RX_END


#endif //ALITA_PASS_H
