//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_VKCANVASCONTEXT_H
#define ALITA_VKCANVASCONTEXT_H

#include "VKDevice.h"


NS_RHI_BEGIN

class VKCanvasContext : public CanvasContext
{
public:
    virtual SwapChain* ConfigureSwapChain(const SwapChainDescriptor &descriptor) override;
    
    virtual TextureFormat GetSwapChainPreferredFormat(Device* device) override;
};

NS_RHI_END


#endif //ALITA_VKCANVASCONTEXT_H
