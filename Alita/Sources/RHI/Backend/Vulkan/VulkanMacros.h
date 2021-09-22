//
// Created by realxie on 2019-10-02.
//

#ifndef RHI_VULKANMARCOS_H
#define RHI_VULKANMARCOS_H

#if ANDROID
#include <android/log.h>
#endif
#include "Macros.h"

#define CALL_VK(func)                                     \
do {                                                      \
    VkResult code = (func);                               \
    if (VK_SUCCESS != code)                               \
    {                                                     \
        DumpVulkanErrorMessage(code, __FILE__, __LINE__); \
        RHI_ASSERT(false);                                \
    }                                                     \
} while (0)

#endif //RHI_VULKANMARCOS_H
