//
// Created by realxie on 2020-01-29.
//

#ifndef RHI_Fence_H
#define RHI_Fence_H

#include "Macros.h"
#include "GFXBase.h"
#include "CommandBuffer.h"

NS_GFX_BEGIN

class Fence : public GfxBase
{
public:
    Fence(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::Fence)
    {}
    
    // Async function.
    void OnCompletion(std::uint64_t completionValue, std::function<void()> onCompletedCallback)
    {
        if (completionValue <= signaledValue_)
        {
            return;
        }
        
        if (completionValue > waitingSignaledValue_)
        {
            LOGE("completion value must be less or equal signal value");
            return;
        }
        
        onCompeletions_.emplace_back(completionValue, onCompletedCallback);
    }
    
    std::uint64_t GetCompletedValue()
    {
        return signaledValue_;
    }
    
    void OnSignaled()
    {
        signaledValue_ = waitingSignaledValue_;
        for (auto &tp : onCompeletions_)
        {
            tp.second();
        }
        onCompeletions_.clear();
    }
    
    void SetWaitingSignalValue(std::uint64_t signalValue)
    {
        waitingSignaledValue_ = signalValue;
    }

    virtual ~Fence() = default;

protected:
    std::uint64_t signaledValue_ = 0;
    std::int64_t waitingSignaledValue_ = -1;
    
    std::vector<std::pair<std::uint64_t, std::function<void()>>> onCompeletions_;
};

NS_GFX_END

#endif //RHI_Fence_H
