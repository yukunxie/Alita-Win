//
// Created by realxie on 2020-02-23.
//

#include "RenderThreading.h"
#include "VKDevice.h"
#include "VKQueue.h"
#include "VKCommandBuffer.h"
#include "VKSwapChain.h"
#include "VKFence.h"
#include "VKBuffer.h"
#include "VKTypes.h"

#include <future>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

NS_RHI_BEGIN

void AsyncWorkerVulkan::PostProcessDoneTask(AsyncTaskPtr task)
{
    device_->SetAsyncTaskDoneInRenderingThread(task);
}

AsyncWorkerVulkan::~AsyncWorkerVulkan()
{
    LOGW("AsyncWorkerVulkan exit begin.");
    RHI_ASSERT(isThreadStopped_);
    device_->GetQueue()->WaitQueueIdle();
    pVulkanAllocator_.reset();
    LOGW("AsyncWorkerVulkan exit end.");
}

AsyncTaskSumbitCommandBufferAndPresent::AsyncTaskSumbitCommandBufferAndPresent(VKDevice* device,
                                                                               FrameResource* frameResource,
                                                                               std::uint32_t commandBufferCount,
                                                                               VKCommandBuffer** commandBuffers)
{
    commandBuffers_.resize(commandBufferCount);
    for (size_t i = 0; i < commandBuffers_.size(); ++i)
    {
        commandBuffers_[i] = commandBuffers[i];
        RHI_SAFE_RETAIN(commandBuffers_[i]);
    }
    
    device_ = device;
    frameResource_ = frameResource;
    queue_ = RHI_CAST(VKQueue*, device_->GetQueue());
}

bool AsyncTaskSumbitCommandBufferAndPresent::RecordCommandBuffer(VkCommandBuffer vkCommandBuffer)
{
    VkCommandBufferBeginInfo cmdBufferBeginInfo;
    {
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufferBeginInfo.pNext = nullptr;
        cmdBufferBeginInfo.flags = 0;
        cmdBufferBeginInfo.pInheritanceInfo = nullptr;
    };
    CALL_VK(vkBeginCommandBuffer(vkCommandBuffer, &cmdBufferBeginInfo));
    
    bool hasDrawCmdInPresentableImage = false;
    // RHI_SCOPED_PROFILING_GUARD("RecordVkCommandBuffer");
    for (int i = 0; i < commandBuffers_.size(); ++i)
    {
        auto cmdBuffer = RHI_CAST(VKCommandBuffer*, commandBuffers_[i]);
        cmdBuffer->BakeCmdBufferAsync(vkCommandBuffer);
        hasDrawCmdInPresentableImage |= cmdBuffer->HasDrawCmdInPresentableImage();
    }
    
    CALL_VK(vkEndCommandBuffer(vkCommandBuffer));
    
    return hasDrawCmdInPresentableImage;
}

void AsyncTaskSumbitCommandBufferAndPresent::WaitingLastSubmissionDone()
{
    auto renderingFrameInfo = frameResource_->swapChain->GetRenderingFrameInfo();
    
    if (renderingFrameInfo && renderingFrameInfo->IsValid())
    {
        VkFence fence = renderingFrameInfo->frameResource->fenceToSyncSubmission;
        VkResult fenceStatus = vkGetFenceStatus(device_->GetNative(), fence);
        if (fenceStatus != VK_SUCCESS)
        {
            // timeout : 2000 ms
            fenceStatus = vkWaitForFences(device_->GetNative(), 1, &fence, VK_TRUE,
                                            2000 * 1000 * 1000);
            if (fenceStatus != VK_SUCCESS)
            {
                LOGE("vkWaitForFences error: %s", GetVkResultString(fenceStatus));
            }
        }
        
        for (size_t i = 0; i < renderingFrameInfo->commandBuffers.size(); ++i)
        {
            auto cmdBuffer = renderingFrameInfo->commandBuffers[i];
            device_->ScheduleCallbackExecutedInGameThread([cmdBuffer](VKDevice* device) {
                auto commandBuffer = cmdBuffer;
                commandBuffer->PostprocessCommandBuffer();
                RHI_SAFE_RELEASE(commandBuffer);
            });
        }
        renderingFrameInfo->commandBuffers.clear();
    }
}

void AsyncTaskSumbitCommandBufferAndPresent::PostprocessLastSubmission()
{
    auto renderingFrameInfo = frameResource_->swapChain->GetRenderingFrameInfo();
    auto asyncWorker = static_cast<AsyncWorkerVulkan*>(((VKDevice*) queue_->GetGPUDevice())->GetAsyncWorker());
    
    // 回收上一帧的VkCommandBuffer
    if (renderingFrameInfo && renderingFrameInfo->IsValid())
    {
        asyncWorker->GetLocalAllocator()->ReleaseVkCommandBuffer(
            renderingFrameInfo->vkCommandBuffer);
        renderingFrameInfo->vkCommandBuffer = VK_NULL_HANDLE;
    }
}

bool AsyncTaskSumbitCommandBufferAndPresent::SumbitCommandBuffer(VkCommandBuffer vkCommandBuffer,
                                                                 VkSemaphore semaWaiting,
                                                                 VkSemaphore semaNotify,
                                                                 VkFence fenceNotify)
{
    VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    
    VkSubmitInfo submitInfo;
    {
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        
        submitInfo.waitSemaphoreCount = semaWaiting != VK_NULL_HANDLE? 1 : 0;
        submitInfo.pWaitSemaphores = &semaWaiting;
        submitInfo.pWaitDstStageMask = &waitStages;
        
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkCommandBuffer;
        
        submitInfo.signalSemaphoreCount = semaNotify != VK_NULL_HANDLE? 1 : 0;
        submitInfo.pSignalSemaphores = &semaNotify;
    }
    
    vkResetFences(device_->GetNative(), 1, &fenceNotify);
    VkResult result = vkQueueSubmit(queue_->GetNative(), 1, &submitInfo, fenceNotify);
    if (VK_SUCCESS != result)
    {
        LOGE("vkQueueSubmit fail code=%s", GetVkResultString(result));
    }
    
    return VK_SUCCESS == result;
}


bool AsyncTaskSumbitCommandBufferAndPresent::Execute()
{
    device_->CheckOnBackgroud();
    
    auto asyncWorker = static_cast<AsyncWorkerVulkan*>(((VKDevice*) queue_->GetGPUDevice())->GetAsyncWorker());
    
    VkCommandBuffer vkCommandBuffer = asyncWorker->GetLocalAllocator()->AcquireVkCommandBuffer();
    
    bool hasDrawCmdInPresentableImage = RecordCommandBuffer(vkCommandBuffer);
    
    WaitingLastSubmissionDone();
    
    auto swapChain_ = device_->GetSwapChain();
    std::uint32_t imageIndex = frameResource_->imageIndex;
    
    auto imageAvailableSemaphore = frameResource_->semaImageAvaliable;
    auto renderFinishedSemaphore = frameResource_->semaPresentable;
    
    if (false == swapChain_->AcquireNextImageKHR(imageAvailableSemaphore, &imageIndex))
    {
        vkResetCommandBuffer(vkCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        RecordCommandBuffer(vkCommandBuffer);
    }
    
    if (false == SumbitCommandBuffer(vkCommandBuffer, imageAvailableSemaphore, renderFinishedSemaphore,
                        frameResource_->fenceToSyncSubmission))
    {
        LOGE("SumbitCommandBuffer fail, retry Execute.");
        asyncWorker->GetLocalAllocator()->ReleaseVkCommandBuffer(vkCommandBuffer);
        Execute();
        return true;
    }
    
    PostprocessLastSubmission();
    
    // 注意，这里将CommandBuffer的所有权转移到RenderingFrameInfo中，commandBuffers_需要clear一下
    swapChain_->SetRenderingFrameInfo(frameResource_, vkCommandBuffer, commandBuffers_.size(),
                                      commandBuffers_.data());
    commandBuffers_.clear();
    
    // present swapchain
    swapChain_->Present(frameResource_->imageIndex, renderFinishedSemaphore, hasDrawCmdInPresentableImage);
    
    return true;
}

AsyncTaskSumbitCommandBufferAndPresent::~AsyncTaskSumbitCommandBufferAndPresent()
{
    for (int i = 0; i < commandBuffers_.size(); ++i)
    {
        auto cmdBuffer = commandBuffers_[i];
        RHI_SAFE_RELEASE(cmdBuffer);
    }
    commandBuffers_.clear();
}

// -------------

AsyncTaskReturnVkSemaphore::AsyncTaskReturnVkSemaphore(VKDevice* device, VkSemaphore semaphore)
    : device_(device), vkSemaphore_(semaphore)
{
}

AsyncTaskReturnVkSemaphore::~AsyncTaskReturnVkSemaphore()
{
    if (device_ && vkSemaphore_)
    {
        device_->ReturnVkSemaphore(vkSemaphore_);
    }
}

// ----

AsyncTaskFenceCompletion::AsyncTaskFenceCompletion(VKDevice* device, VKQueue* queue, VKFence* fence)
    : device_(device)
{
    RHI_PTR_ASSIGN(fence_, fence);
}

AsyncTaskFenceCompletion::~AsyncTaskFenceCompletion()
{
    RHI_SAFE_RELEASE(fence_);
}

bool AsyncTaskFenceCompletion::Execute()
{
    // Notice, the callback must be executed ahead of the destructor.
    device_->ScheduleCallbackExecutedInGameThread([this](VKDevice*) {
        fence_->OnSignaled();
    });
    return true;
}

// ---

AsyncTaskBufferMapRead::AsyncTaskBufferMapRead(VKDevice* device, VKBuffer* buffer, std::uint32_t offset, std::uint32_t size)
{
    device_ = device;
    offset_ = offset;
    size_ = size;
    RHI_PTR_ASSIGN(buffer_, buffer);
}

AsyncTaskBufferMapRead::~AsyncTaskBufferMapRead()
{
    RHI_SAFE_RELEASE(buffer_);
}

bool AsyncTaskBufferMapRead::Execute()
{
    device_->ScheduleCallbackExecutedInGameThread([this](VKDevice*) {
        buffer_->CallMapReadCallback();
    });
    return true;
}

// ----

AsyncTaskBufferMapWrite::AsyncTaskBufferMapWrite(VKDevice* device, VKBuffer* buffer, std::uint32_t offset, std::uint32_t size)
{
    device_ = device;
    offset_ = offset;
    size_ = size;
    RHI_PTR_ASSIGN(buffer_, buffer);
}

AsyncTaskBufferMapWrite::~AsyncTaskBufferMapWrite()
{
    RHI_SAFE_RELEASE(buffer_);
}

bool AsyncTaskBufferMapWrite::Execute()
{
    device_->ScheduleCallbackExecutedInGameThread([this](VKDevice*) {
        buffer_->CallMapWriteCallback();
    });
    return true;
}

// ----

NS_RHI_END


