//
// Created by realxie on 2019-10-10.
//

#ifndef ALITA_BINDGROUP_H
#define ALITA_BINDGROUP_H

#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class BindGroup : public RHIObjectBase
{
public:
    virtual ~BindGroup()
    {}
};

NS_RHI_END

#endif //ALITA_BINDGROUP_H
