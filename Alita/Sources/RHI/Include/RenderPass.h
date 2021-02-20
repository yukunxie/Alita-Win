//
// Created by realxie on 2019-10-04.
//

#ifndef ALITA_RENDERPASS_H
#define ALITA_RENDERPASS_H

#include "Macros.h"
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class RenderPass : public RHIObjectBase
{
public:
    virtual ~RenderPass() = default;
};

NS_RHI_END

#endif //ALITA_RENDERPASS_H
