//
// Created by realxie on 2020-01-29.
//

#ifndef RHI_VKFence_H
#define RHI_VKFence_H

#include "VKDevice.h"
#include <thread>
#include <mutex>
#include <condition_variable>

NS_GFX_BEGIN

class VKFence final : public Fence
{
protected:
    VKFence() = delete;
    
    VKFence(VKDevice* device);
    
    virtual ~VKFence();

public:
    bool Init(const FenceDescriptor& descriptor);
    
    virtual void Dispose() override;

private:
    VKDevice* device_ = nullptr;
    
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKFence_H
