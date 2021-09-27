//
// Created by realxie on 2019-10-21.
//

#ifndef RHI_SWAPCHAIN_H
#define RHI_SWAPCHAIN_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "Texture.h"
#include "Queue.h"


NS_RHI_BEGIN

class SwapChain : public RHIObjectBase
{
public:
    SwapChain(Device* GPUDevice)
        : RHIObjectBase(GPUDevice, RHIObjectType::SwapChain)
    {}

protected:
    virtual ~SwapChain() = default;

public:
    
    virtual Texture* GetCurrentTexture() = 0;
    
    // virtual void Present(Queue* queue) = 0;
    
    virtual Extent2D GetExtent() = 0;
    
    virtual TextureFormat GetFormat() = 0;

    virtual void RecreateSwapChain() = 0;
};

NS_RHI_END

#endif //RHI_SWAPCHAIN_H
