//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_CANVASCONTEXT_H
#define ALITA_CANVASCONTEXT_H

#include "RHIObjectBase.h"
#include "SwapChain.h"
#include "Descriptors.h"
#include "Device.h"

NS_RHI_BEGIN

class CanvasContext
{
public:
    virtual SwapChain* ConfigureSwapChain(const SwapChainDescriptor& descriptor) = 0;
    
    virtual TextureFormat GetSwapChainPreferredFormat(Device* device) = 0;
};

NS_RHI_END


#endif //ALITA_CANVASCONTEXT_H
