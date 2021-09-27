//
// Created by realxie on 2020-02-23.
//

#ifndef PROJ_ANDROID_RENDERTHREADING_H
#define PROJ_ANDROID_RENDERTHREADING_H


#include "VKDevice.h"
#include "VKFence.h"
#include "VKSwapChain.h"
#include "RenderThreadingVkAllocator.h"

NS_RHI_BEGIN

class VKDevice;

class VKQueue;

class VKSwapChain;

class AsyncWorkerVulkan final : public AsyncWorker
{
public:
    AsyncWorkerVulkan(VKDevice* device)
        : AsyncWorker(), device_(device)
    {
        pVulkanAllocator_ = std::make_unique<ThreadLocalVulkanAllocator>(device_->GetNative());
    }
    
    virtual void CheckForeground() override
    {
        device_->CheckOnBackgroud();
    }
    
    ThreadLocalVulkanAllocator* GetLocalAllocator()
    { return pVulkanAllocator_.get(); }
    
    ~AsyncWorkerVulkan();

protected:
    virtual void PostProcessDoneTask(AsyncTaskPtr task) override;

private:
    VKDevice* device_ = nullptr;
    std::unique_ptr<ThreadLocalVulkanAllocator> pVulkanAllocator_ = nullptr;
};


struct AsyncTaskSumbitCommandBufferAndPresent : public AsyncTask
{
    AsyncTaskSumbitCommandBufferAndPresent(VKDevice* device,
                                           FrameResource* frameResource,
                                           std::uint32_t commandBufferCount,
                                           VKCommandBuffer** commandBuffers);
    
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
    VKDevice* device_ = nullptr;
    VKQueue* queue_ = nullptr;
    std::vector<VKCommandBuffer*> commandBuffers_;
    FrameResource* frameResource_;
};

struct AsyncTaskReturnVkSemaphore : public AsyncTask
{
    AsyncTaskReturnVkSemaphore(VKDevice* device, VkSemaphore semaphore);
    
    virtual bool Execute() override
    { return true; }
    
    virtual const char* GetName() override
    {
        return "AsyncTaskReturnVkSemaphore";
    }
    
    virtual ~AsyncTaskReturnVkSemaphore();

private:
    VKDevice* device_ = nullptr;
    VkSemaphore vkSemaphore_ = VK_NULL_HANDLE;
};

struct AsyncTaskFenceCompletion : AsyncTask
{
    AsyncTaskFenceCompletion(VKDevice* device, VKQueue* queue, VKFence* fence);
    
    ~AsyncTaskFenceCompletion();
    
    virtual bool Execute() override;
    
    virtual const char* GetName() override
    {
        return "AsyncTaskFenceCompletion";
    }

protected:
    VKDevice* device_ = nullptr;
    VKFence* fence_ = nullptr;
};

struct AsyncTaskBufferMapRead : AsyncTask
{
    AsyncTaskBufferMapRead(VKDevice* device, VKBuffer* buffer, std::uint32_t offset = 0, std::uint32_t size = 0);
    
    ~AsyncTaskBufferMapRead();
    
    virtual bool Execute() override;
    
    virtual const char* GetName() override
    {
        return "AsyncTaskBufferMapRead";
    }

protected:
    VKDevice* device_ = nullptr;
    VKBuffer* buffer_ = nullptr;

    std::uint32_t offset_ = 0;
    std::uint32_t size_ = 0;
};

struct AsyncTaskBufferMapWrite : AsyncTask
{
    AsyncTaskBufferMapWrite(VKDevice* device, VKBuffer* buffer, std::uint32_t offset = 0, std::uint32_t size = 0);
    
    ~AsyncTaskBufferMapWrite();
    
    virtual bool Execute() override;
    
    virtual const char* GetName() override
    {
        return "AsyncTaskBufferMapWrite";
    }

protected:
    VKDevice* device_ = nullptr;
    VKBuffer* buffer_ = nullptr;

    std::uint32_t offset_ = 0;
    std::uint32_t size_ = 0;
};

NS_RHI_END

#endif //PROJ_ANDROID_RENDERTHREADING_H
