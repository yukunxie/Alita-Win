//
// Created by realxie on 2019-10-11.
//

#ifndef RHI_VKRENDERQUEUE_H
#define RHI_VKRENDERQUEUE_H

#include "VKDevice.h"

NS_GFX_BEGIN

class VKRenderQueue final : public RenderQueue
{
protected:
    VKRenderQueue(const DevicePtr& device);
  
public:
    virtual ~VKRenderQueue();

public:
    
    bool Init()
    {
        return true;
    }
    
    virtual void Dispose() override;
    
    virtual void Submit(const CommandBuffer* commandBuffer) override
    {
        GFX_ASSERT(false, "unimplemented.");
    }

private:
    friend class VKDevice;
};

NS_GFX_END


#endif //RHI_VKRENDERQUEUE_H
