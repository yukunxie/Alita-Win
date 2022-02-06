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

NS_GFX_BEGIN

void AsyncWorkerVulkan::PostProcessDoneTask(AsyncTaskPtr task)
{
    GFX_CAST(VKDevice*, device_)->SetAsyncTaskDoneInRenderingThread(task);
}

AsyncWorkerVulkan::~AsyncWorkerVulkan()
{
    LOGW("AsyncWorkerVulkan exit begin.");
    GFX_ASSERT(isThreadStopped_);
    device_->GetQueue()->WaitQueueIdle();
    pVulkanAllocator_.reset();
    LOGW("AsyncWorkerVulkan exit end.");
}

AsyncTaskSumbitCommandBufferAndPresent::AsyncTaskSumbitCommandBufferAndPresent(const DevicePtr& device,
                                                                               FrameResource* frameResource,
                                                                               std::uint32_t commandBufferCount,
                                                                               CommandBufferPtr* commandBuffers)
{
    commandBuffers_.resize(commandBufferCount);
    for (size_t i = 0; i < commandBuffers_.size(); ++i)
    {
        commandBuffers_[i] = commandBuffers[i];
        GFX_SAFE_RETAIN(commandBuffers_[i]);
    }
    
    device_ = device;
    frameResource_ = frameResource;
    queue_ = device_->GetQueue();
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
        auto cmdBuffer = GFX_CAST(VKCommandBuffer*, commandBuffers_[i]);
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
        VkResult fenceStatus = vkGetFenceStatus(GFX_CAST(VKDevice*, device_)->GetNative(), fence);
        if (fenceStatus != VK_SUCCESS)
        {
            // timeout : 2000 ms
            fenceStatus = vkWaitForFences(GFX_CAST(VKDevice*, device_)->GetNative(), 1, &fence, VK_TRUE,
                                            2000 * 1000 * 1000);
            if (fenceStatus != VK_SUCCESS)
            {
                LOGE("vkWaitForFences error: %s", GetVkResultString(fenceStatus));
            }
        }
        
        for (size_t i = 0; i < renderingFrameInfo->commandBuffers.size(); ++i)
        {
            auto cmdBuffer = renderingFrameInfo->commandBuffers[i];
            GFX_CAST(VKDevice*, device_)->ScheduleCallbackExecutedInGameThread([cmdBuffer](const DevicePtr& device) {
                GFX_CAST(VKCommandBuffer*, cmdBuffer)->PostprocessCommandBuffer();
            });
        }
        renderingFrameInfo->commandBuffers.clear();
    }
}

void AsyncTaskSumbitCommandBufferAndPresent::PostprocessLastSubmission()
{
    auto renderingFrameInfo = frameResource_->swapChain->GetRenderingFrameInfo();
    auto asyncWorker = static_cast<AsyncWorkerVulkan*>(GFX_CAST(VKDevice*, queue_->GetGPUDevice())->GetAsyncWorker());
    
    // 回收上一帧的VkCommandBuffer
    if (renderingFrameInfo && renderingFrameInfo->IsValid())
    {
        asyncWorker->GetLocalAllocator()->ReleaseVkCommandBuffer(
            renderingFrameInfo->vkCommandBuffer);
        renderingFrameInfo->vkCommandBuffer = VK_NULL_HANDLE;
    }
}

std::uint32_t AsyncTaskSumbitCommandBufferAndPresent::SumbitCommandBuffer(VkCommandBuffer vkCommandBuffer,
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
    
    if (VkResult result = vkResetFences(GFX_CAST(VKDevice*, device_)->GetNative(), 1, &fenceNotify); VK_SUCCESS != result)
    {
        LOGE("vkQueueSubmit-vkResetFences fail code=%s", GetVkResultString(result));
        return result;
    }

    if (VkResult result = vkQueueSubmit(GFX_CAST(VKQueue*, queue_)->GetNative(), 1, &submitInfo, fenceNotify); VK_SUCCESS != result)
    {
        LOGE("vkQueueSubmit-vkQueueSubmit fail code=%s", GetVkResultString(result));
        return result;
    }
    
    return VkResult::VK_SUCCESS;
}


bool AsyncTaskSumbitCommandBufferAndPresent::Execute()
{
    GFX_CAST(VKDevice*, device_)->CheckOnBackgroud();
    
    auto asyncWorker = static_cast<AsyncWorkerVulkan*>(GFX_CAST(VKDevice*, queue_->GetGPUDevice())->GetAsyncWorker());
    
    VkCommandBuffer vkCommandBuffer = asyncWorker->GetLocalAllocator()->AcquireVkCommandBuffer();
    
    bool hasDrawCmdInPresentableImage = RecordCommandBuffer(vkCommandBuffer);
    
    WaitingLastSubmissionDone();
    
    auto swapChain_ = GFX_CAST(VKSwapChain*, GFX_CAST(VKDevice*, device_)->GetSwapChain());
    std::uint32_t imageIndex = frameResource_->imageIndex;
    
    auto imageAvailableSemaphore = frameResource_->semaImageAvaliable;
    auto renderFinishedSemaphore = frameResource_->semaPresentable;
    
    if (false == swapChain_->AcquireNextImageKHR(imageAvailableSemaphore, &imageIndex))
    {
        vkResetCommandBuffer(vkCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        RecordCommandBuffer(vkCommandBuffer);
    }

    VkResult result = (VkResult)SumbitCommandBuffer(vkCommandBuffer, imageAvailableSemaphore, renderFinishedSemaphore,
        frameResource_->fenceToSyncSubmission);

    if (result == VK_ERROR_DEVICE_LOST)
    {
        LOGE("SumbitCommandBuffer fail, RecreateSwapChain.");
        GFX_CAST(VKDevice*, device_)->GetSwapChain()->RecreateSwapChain();
    }

    if (result != VK_SUCCESS)
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
        GFX_SAFE_RELEASE(cmdBuffer);
    }
    commandBuffers_.clear();
}

// -------------

AsyncTaskReturnVkSemaphore::AsyncTaskReturnVkSemaphore(const DevicePtr& device, VkSemaphore semaphore)
    : device_(device), vkSemaphore_(semaphore)
{
}

AsyncTaskReturnVkSemaphore::~AsyncTaskReturnVkSemaphore()
{
    if (device_ && vkSemaphore_)
    {
        GFX_CAST(VKDevice*, device_)->ReturnVkSemaphore(vkSemaphore_);
    }
}

// ----

AsyncTaskFenceCompletion::AsyncTaskFenceCompletion(const DevicePtr& device, const QueuePtr& queue, const FencePtr& fence)
    : device_(device)
{
    fence_ = fence;
}

AsyncTaskFenceCompletion::~AsyncTaskFenceCompletion()
{
}

bool AsyncTaskFenceCompletion::Execute()
{
    // Notice, the callback must be executed ahead of the destructor.
    GFX_CAST(VKDevice*, device_)->ScheduleCallbackExecutedInGameThread([this](DevicePtr) {
        fence_->OnSignaled();
    });
    return true;
}

// ---

AsyncTaskBufferMapRead::AsyncTaskBufferMapRead(const DevicePtr& device, const BufferPtr& buffer, std::uint32_t offset, std::uint32_t size)
{
    device_ = device;
    offset_ = offset;
    size_ = size;
    buffer_ = buffer;
}

AsyncTaskBufferMapRead::~AsyncTaskBufferMapRead()
{
}

bool AsyncTaskBufferMapRead::Execute()
{
    GFX_CAST(VKDevice*, device_)->ScheduleCallbackExecutedInGameThread([this](DevicePtr device) {
        GFX_CAST(VKBuffer*, buffer_)->CallMapReadCallback();
    });
    return true;
}

// ----

AsyncTaskBufferMapWrite::AsyncTaskBufferMapWrite(const DevicePtr& device, const BufferPtr& buffer, std::uint32_t offset, std::uint32_t size)
{
    device_ = device;
    offset_ = offset;
    size_ = size;
    buffer_ = buffer;
}

AsyncTaskBufferMapWrite::~AsyncTaskBufferMapWrite()
{
}

bool AsyncTaskBufferMapWrite::Execute()
{
    GFX_CAST(VKDevice*, device_)->ScheduleCallbackExecutedInGameThread([this](DevicePtr device) {
        GFX_CAST(VKBuffer*, buffer_)->CallMapWriteCallback();
    });
    return true;
}

// ----

NS_GFX_END


