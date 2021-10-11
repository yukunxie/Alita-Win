//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_CANVASCONTEXT_H
#define ALITA_CANVASCONTEXT_H

#include "GFXBase.h"
#include "SwapChain.h"
#include "Descriptors.h"
#include "Device.h"

NS_GFX_BEGIN

class CanvasContext
{
public:
    virtual SwapChain* ConfigureSwapChain(const SwapChainDescriptor& descriptor) = 0;
    
    virtual TextureFormat GetSwapChainPreferredFormat(Device* device) = 0;
};

NS_GFX_END


#endif //ALITA_CANVASCONTEXT_H
