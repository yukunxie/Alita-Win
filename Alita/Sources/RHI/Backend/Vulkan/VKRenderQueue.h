//
// Created by realxie on 2019-10-11.
//

#ifndef ALITA_VKRENDERQUEUE_H
#define ALITA_VKRENDERQUEUE_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKRenderQueue : public RenderQueue
{
public:
    VKRenderQueue();
    
    virtual ~VKRenderQueue();
};

NS_RHI_END


#endif //ALITA_VKRENDERQUEUE_H
