//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_TECHNIQUE_H
#define ALITA_TECHNIQUE_H

#include "../Base/ObjectBase.h"
#include "Pass.h"

NS_RX_BEGIN

class Technique
{
protected:
    std::list<Pass*> renderPasses_;
};

NS_RX_END

#endif //ALITA_TECHNIQUE_H
