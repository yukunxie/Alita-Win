//
// Created by realxie on 2019-10-18.
//

#include "VKQueue.h"
#include "VKCommandBuffer.h"
#include "VKFence.h"
#include "VKSwapChain.h"
#include "RenderThreading.h"
#include <chrono>

NS_GFX_BEGIN

VKQueue::VKQueue(const DevicePtr& device)
    : Queue(device)
{
}

void VKQueue::Dispose()
{
    GFX_DISPOSE_BEGIN();
    
    for (auto cmdBuffer : commandBufferCaches_)
    {
        GFX_SAFE_RELEASE(cmdBuffer);
    }
    commandBufferCaches_.clear();
    
    if (vkQueue_)
    {
        // vkQueueWaitIdle(vkQueue_);
        vkQueue_ = VK_NULL_HANDLE;
    }
    
    GFX_DISPOSE_END();
}

VKQueue::~VKQueue()
{
    Dispose();
}

bool VKQueue::Init()
{
    auto device = VKDEVICE();
    vkGetDeviceQueue(device->GetNative(), device->GetQueueFamilyIndices().presentFamily, 0,
                       &vkQueue_);
    return VK_NULL_HANDLE != vkQueue_;
}

void VKQueue::Signal(const FencePtr& fence, std::uint64_t signalValue)
{
    auto it = std::find(waitingFences_.begin(), waitingFences_.end(), fence);
    if (it == waitingFences_.end())
    {
        waitingFences_.push_back(fence);
    }
    fence->SetWaitingSignalValue(signalValue);
}

void VKQueue::WaitQueueIdle()
{
    vkQueueWaitIdle(vkQueue_);
}

void VKQueue::Submit(std::uint32_t commandBufferCount, CommandBufferPtr const* commandBuffers)
{
    for (int i = 0; i < commandBufferCount; ++i)
    {
        commandBufferCaches_.push_back(commandBuffers[i]);
    }
}

void VKQueue::SubmitInternal()
{
    if (commandBufferCaches_.empty())
    {
        return;
    }
    
    VKDevice* device = VKDEVICE();
    VKSwapChain* swapChain = GFX_CAST(VKSwapChain*, device->GetSwapChain());
    FrameResource* frameResource = swapChain->GetFrameResource();
    
    device->ScheduleAsyncTask<AsyncTaskSumbitCommandBufferAndPresent>(GetGPUDevice(), 
        frameResource,
        commandBufferCaches_.size(),
        commandBufferCaches_.data());

    commandBufferCaches_.clear();
    
    imageLayoutInitCommandBuffer_ = nullptr;
    
    for (auto fence : waitingFences_)
    {
        device->ScheduleAsyncTask<AsyncTaskFenceCompletion>(GetGPUDevice(), GetShared(), fence);
    }
    waitingFences_.clear();
    
    swapChain->AcquireNextImage();
}

FencePtr VKQueue::CreateFence(const FenceDescriptor &descriptor)
{
    return VKDEVICE()->CreateFence(descriptor);
}

CommandBufferPtr VKQueue::GetImageLayoutInitCommandBuffer()
{
    if (!imageLayoutInitCommandBuffer_)
    {
        imageLayoutInitCommandBuffer_ = VKDEVICE()->CreateCommandBuffer();
        GFX_SAFE_RETAIN(imageLayoutInitCommandBuffer_);
        commandBufferCaches_.insert(commandBufferCaches_.begin(),imageLayoutInitCommandBuffer_);
    }
    return imageLayoutInitCommandBuffer_;
}

NS_GFX_END