//
// Created by realxie on 2020-02-23.
//

#ifndef PROJ_ANDROID_RENDERTHREADING_H
#define PROJ_ANDROID_RENDERTHREADING_H


#include "VKDevice.h"
#include "VKFence.h"
#include "VKSwapChain.h"
#include "RenderThreadingVkAllocator.h"

NS_GFX_BEGIN

class VKDevice;

class VKQueue;

class VKSwapChain;

class AsyncWorkerVulkan final : public AsyncWorker
{
public:
    AsyncWorkerVulkan(const DevicePtr& device)
        : AsyncWorker(), device_(device)
    {
        pVulkanAllocator_ = std::make_unique<ThreadLocalVulkanAllocator>(GFX_CAST(VKDevice*, device_)->GetNative());
    }
    
    virtual void CheckForeground() override
    {
        GFX_CAST(VKDevice*, device_)->CheckOnBackgroud();
    }
    
    ThreadLocalVulkanAllocator* GetLocalAllocator()
    { return pVulkanAllocator_.get(); }
    
    ~AsyncWorkerVulkan();

protected:
    virtual void PostProcessDoneTask(AsyncTaskPtr task) override;

private:
    DevicePtr device_ = nullptr;
    std::unique_ptr<ThreadLocalVulkanAllocator> pVulkanAllocator_ = nullptr;
};


struct AsyncTaskSumbitCommandBufferAndPresent : public AsyncTask
{
    AsyncTaskSumbitCommandBufferAndPresent(const DevicePtr& device,
                                           FrameResource* frameResource,
                                           std::uint32_t commandBufferCount,
                                           CommandBufferPtr* commandBuffers);
    
    bool RecordCommandBuffer(VkCommandBuffer vkCommandBuffer);
    
    void WaitingLastSubmissionDone();
    
    void PostprocessLastSubmission();
    
    std::uint32_t SumbitCommandBuffer(VkCommandBuffer vkCommandBuffer, VkSemaphore semaWaiting,
                             VkSemaphore semaNotify, VkFence fenceNotify);
    
    virtual bool Execute() override;
    
    virtual const char* GetName() override
    {
        return "AsyncTaskSumbitCommandBufferAndPresent";
    }
    
    ~AsyncTaskSumbitCommandBufferAndPresent();

protected:
    DevicePtr device_ = nullptr;
    QueuePtr queue_ = nullptr;
    std::vector<CommandBufferPtr> commandBuffers_;
    FrameResource* frameResource_;
};

struct AsyncTaskReturnVkSemaphore : public AsyncTask
{
    AsyncTaskReturnVkSemaphore(const DevicePtr& device, VkSemaphore semaphore);
    
    virtual bool Execute() override
    { return true; }
    
    virtual const char* GetName() override
    {
        return "AsyncTaskReturnVkSemaphore";
    }
    
    virtual ~AsyncTaskReturnVkSemaphore();

private:
    DevicePtr device_ = nullptr;
    VkSemaphore vkSemaphore_ = VK_NULL_HANDLE;
};

struct AsyncTaskFenceCompletion : AsyncTask
{
    AsyncTaskFenceCompletion(const DevicePtr& device, const QueuePtr& queue, const FencePtr& fence);
    
    ~AsyncTaskFenceCompletion();
    
    virtual bool Execute() override;
    
    virtual const char* GetName() override
    {
        return "AsyncTaskFenceCompletion";
    }

protected:
    DevicePtr device_ = nullptr;
    FencePtr fence_ = nullptr;
};

struct AsyncTaskBufferMapRead : AsyncTask
{
    AsyncTaskBufferMapRead(const DevicePtr& device, const BufferPtr& buffer, std::uint32_t offset = 0, std::uint32_t size = 0);
    
    ~AsyncTaskBufferMapRead();
    
    virtual bool Execute() override;
    
    virtual const char* GetName() override
    {
        return "AsyncTaskBufferMapRead";
    }

protected:
    DevicePtr device_ = nullptr;
    BufferPtr buffer_ = nullptr;

    std::uint32_t offset_ = 0;
    std::uint32_t size_ = 0;
};

struct AsyncTaskBufferMapWrite : AsyncTask
{
    AsyncTaskBufferMapWrite(const DevicePtr& device, const BufferPtr& buffer, std::uint32_t offset = 0, std::uint32_t size = 0);
    
    ~AsyncTaskBufferMapWrite();
    
    virtual bool Execute() override;
    
    virtual const char* GetName() override
    {
        return "AsyncTaskBufferMapWrite";
    }

protected:
    DevicePtr device_ = nullptr;
    BufferPtr buffer_ = nullptr;

    std::uint32_t offset_ = 0;
    std::uint32_t size_ = 0;
};

NS_GFX_END

#endif //PROJ_ANDROID_RENDERTHREADING_H
