//
// Created by realxie on 2019-10-14.
//

#ifndef ALITA_VKCOMMANDBUFFER_H
#define ALITA_VKCOMMANDBUFFER_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKCommandBuffer : public CommandBuffer
{
public:
    VKCommandBuffer(VKDevice* device);
    
    virtual ~VKCommandBuffer();
    
    VkCommandBuffer GetNative() const
    { return vkCommandBuffer_; }
    
    void ResetCommandBuffer();

private:
    VKDevice* device_ = nullptr;
    VkCommandBuffer vkCommandBuffer_;
};

NS_RHI_END


#endif //ALITA_VKCOMMANDBUFFER_H
