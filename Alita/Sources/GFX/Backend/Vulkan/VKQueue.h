//
// Created by realxie on 2019-10-18.
//

#ifndef RHI_VKQUEUE_H
#define RHI_VKQUEUE_H

#include "VKDevice.h"

NS_GFX_BEGIN

class VKQueue final : public Queue
{
protected:
    VKQueue(const DevicePtr& device);
    
public:
    virtual ~VKQueue();

public:
    
    bool Init();
    
    VkQueue GetNative() const
    { return vkQueue_; }

public:
    virtual void
    Submit(std::uint32_t commandBufferCount, CommandBufferPtr const* commandBuffers) override;
    
    void SubmitInternal();
    
    virtual FencePtr CreateFence(const FenceDescriptor &descriptor) override;
    
    virtual void Dispose() override;
    
    virtual void Signal(const FencePtr& fence, std::uint64_t signalValue) override;
    
    virtual void WaitQueueIdle() override;
    
    CommandBufferPtr GetImageLayoutInitCommandBuffer();
    
private:
    std::vector<FencePtr> waitingFences_;
    VkQueue vkQueue_ = VK_NULL_HANDLE;
    VkFence vkFence_ = VK_NULL_HANDLE;
    
    CommandBufferPtr imageLayoutInitCommandBuffer_ = nullptr;
    std::vector<CommandBufferPtr> commandBufferCaches_;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKQUEUE_H
