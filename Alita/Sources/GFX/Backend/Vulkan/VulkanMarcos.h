//
// Created by realxie on 2019-10-02.
//

#ifndef ALITA_VULKANMARCOS_H
#define ALITA_VULKANMARCOS_H


#include "Platform/Platform.h"

#include "Macros.h"


#ifndef CALL_VK
#define CALL_VK(func)                                                                                       \
  do {                                                                                                      \
    if (auto code = (func); VK_SUCCESS != code)                                                             \
    {                                                                                                       \
        const char* message = "UNDEFINED";                                                                  \
        switch(code)                                                                                        \
        {                                                                                                   \
            case VK_SUCCESS: message = "VK_SUCCESS";break;                                                  \
            case VK_NOT_READY: message = "VK_NOT_READY";break;                                              \
            case VK_TIMEOUT: message = "VK_TIMEOUT";break;                                                  \
            case VK_EVENT_SET: message = "VK_EVENT_SET";break;                                              \
            case VK_EVENT_RESET: message = "VK_EVENT_RESET";break;                                          \
            case VK_INCOMPLETE: message = "VK_INCOMPLETE";break;                                            \
            case VK_ERROR_OUT_OF_HOST_MEMORY: message = "VK_ERROR_OUT_OF_HOST_MEMORY";break;                \
            case VK_ERROR_OUT_OF_DEVICE_MEMORY: message = "VK_ERROR_OUT_OF_DEVICE_MEMORY";break;            \
            case VK_ERROR_INITIALIZATION_FAILED: message = "VK_ERROR_INITIALIZATION_FAILED";break;          \
            case VK_ERROR_DEVICE_LOST: message = "VK_ERROR_DEVICE_LOST";break;                              \
            case VK_ERROR_MEMORY_MAP_FAILED: message = "VK_ERROR_MEMORY_MAP_FAILED";break;                  \
            case VK_ERROR_LAYER_NOT_PRESENT: message = "VK_ERROR_LAYER_NOT_PRESENT";break;                  \
            case VK_ERROR_EXTENSION_NOT_PRESENT: message = "VK_ERROR_EXTENSION_NOT_PRESENT";break;          \
            case VK_ERROR_FEATURE_NOT_PRESENT: message = "VK_ERROR_FEATURE_NOT_PRESENT";break;              \
            case VK_ERROR_INCOMPATIBLE_DRIVER: message = "VK_ERROR_INCOMPATIBLE_DRIVER";break;              \
            case VK_ERROR_TOO_MANY_OBJECTS: message = "VK_ERROR_TOO_MANY_OBJECTS";break;                    \
            case VK_ERROR_FORMAT_NOT_SUPPORTED: message = "VK_ERROR_FORMAT_NOT_SUPPORTED";break;            \
            case VK_ERROR_FRAGMENTED_POOL: message = "VK_ERROR_FRAGMENTED_POOL";break;                      \
            case VK_ERROR_OUT_OF_POOL_MEMORY: message = "VK_ERROR_OUT_OF_POOL_MEMORY";break;                \
            case VK_ERROR_INVALID_EXTERNAL_HANDLE: message = "VK_ERROR_INVALID_EXTERNAL_HANDLE";break;      \
            case VK_ERROR_SURFACE_LOST_KHR: message = "VK_ERROR_SURFACE_LOST_KHR";break;                    \
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: message = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";break;    \
            case VK_ERROR_OUT_OF_DATE_KHR: message = "VK_ERROR_OUT_OF_DATE_KHR";break;                      \
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: message = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";break;    \
            case VK_ERROR_VALIDATION_FAILED_EXT: message = "VK_ERROR_VALIDATION_FAILED_EXT";break;          \
            case VK_ERROR_INVALID_SHADER_NV: message = "VK_ERROR_INVALID_SHADER_NV";break;                  \
            case VK_ERROR_FRAGMENTATION_EXT: message = "VK_ERROR_FRAGMENTATION_EXT";break;                  \
            case VK_ERROR_NOT_PERMITTED_EXT: message = "VK_ERROR_NOT_PERMITTED_EXT";break;                  \
            default: break;                                                                                 \
        }                                                                                                   \
        LOGE("Vulkan error [%s - %d]. File[%s], line[%d] \n", message, code, __FILE__,                      \
                        __LINE__);                                                                          \
                        Assert(false);                                                                      \
    }                                                                                                       \
} while (0)
#endif

#endif //ALITA_VULKANMARCOS_H
