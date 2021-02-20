//
// Created by realxie on 2019-10-29.
//

#include "VKCanvasContext.h"
#include "VKSwapChain.h"

NS_RHI_BEGIN

SwapChain* VKCanvasContext::ConfigureSwapChain(const SwapChainDescriptor &descriptor)
{
    auto vkDevice = RHI_CAST(VKDevice*, descriptor.device);
    auto swapChain = new VKSwapChain(vkDevice, GetVkFormat(descriptor.format));
    RHI_SAFE_RETAIN(swapChain);
    return swapChain;
}

TextureFormat VKCanvasContext::GetSwapChainPreferredFormat(Device* device)
{
    RHI_ASSERT(false);
    return TextureFormat::RGBA8UNORM;
}


NS_RHI_END