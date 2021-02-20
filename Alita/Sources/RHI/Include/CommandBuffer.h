//
// Created by realxie on 2019-10-11.
//

#ifndef ALITA_COMMANDBUFFER_H
#define ALITA_COMMANDBUFFER_H

#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class CommandBuffer : public RHIObjectBase
{
public:
    virtual ~CommandBuffer()
    {}
};

NS_RHI_END


#endif //ALITA_COMMANDBUFFER_H
