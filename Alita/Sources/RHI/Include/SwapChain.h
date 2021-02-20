//
// Created by realxie on 2019-10-21.
//

#ifndef ALITA_SWAPCHAIN_H
#define ALITA_SWAPCHAIN_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "TextureView.h"
#include "Queue.h"


NS_RHI_BEGIN

class SwapChain : public RHIObjectBase
{
public:
    virtual ~SwapChain() = default;
    
    virtual TextureView* GetCurrentTexture() = 0;
    
    virtual void Present(const Queue* queue) = 0;
    
    virtual Extent2D GetExtent() = 0;
    
    virtual TextureFormat GetFormat() = 0;
};

NS_RHI_END

#endif //ALITA_SWAPCHAIN_H
