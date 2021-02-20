//
// Created by realxie on 2019-10-18.
//

#ifndef ALITA_VKQUEUE_H
#define ALITA_VKQUEUE_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKQueue : public Queue
{
public:
    VKQueue(VKDevice* device);
    
    virtual ~VKQueue();
    
    VkQueue GetNative() const
    { return vkQueue_; }

public:
    virtual void Submit(CommandBuffer* commandBuffer) override;

private:
    VKDevice* device_ = nullptr;
    VkDevice vkDevice_ = nullptr;
    VkQueue vkQueue_ = 0L;
    VkFence vkFence_ = 0L;
};

NS_RHI_END


#endif //ALITA_VKQUEUE_H
