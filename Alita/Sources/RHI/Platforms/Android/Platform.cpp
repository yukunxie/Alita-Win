//
// Created by realxie on 2019-11-03.
//

#include "Platform.h"
#include "../../Include/RHI.h"
#include "../../backend/Vulkan/VKDevice.h"

#include <android/native_window.h>

NS_RHI_BEGIN

Device* CreateDeviceHelper(DeviceType deviceType, void* data)
{
    ANativeWindow* window = static_cast<ANativeWindow*>(data);
    return new VKDevice(window);
    
}

NS_RHI_END
