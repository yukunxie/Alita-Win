//
// Created by realxie on 2019-10-18.
//

#ifndef RHI_VKQUEUE_H
#define RHI_VKQUEUE_H

#include "VKDevice.h"

NS_RHI_BEGIN

class VKQueue final : public Queue
{
protected:
    VKQueue(VKDevice* device);
    
    virtual ~VKQueue();

public:
    
    bool Init();
    
    VkQueue GetNative() const
    { return vkQueue_; }

public:
    virtual void
    Submit(std::uint32_t commandBufferCount, CommandBuffer* const* commandBuffers) override;
    
    void SubmitInternal();
    
    virtual Fence* CreateFence(const FenceDescriptor &descriptor) override;
    
    virtual void Dispose() override;
    
    virtual void Signal(Fence* fence, std::uint64_t signalValue) override;
    
    virtual void WaitQueueIdle() override;
    
    VKCommandBuffer* GetImageLayoutInitCommandBuffer();
    
private:
    std::vector<Fence*> waitingFences_;
    VkQueue vkQueue_ = VK_NULL_HANDLE;
    VkFence vkFence_ = VK_NULL_HANDLE;
    
    VKCommandBuffer* imageLayoutInitCommandBuffer_ = nullptr;
    std::vector<VKCommandBuffer*> commandBufferCaches_;
    
    friend class VKDevice;
};

NS_RHI_END


#endif //RHI_VKQUEUE_H
