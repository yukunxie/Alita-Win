//
// Created by realxie on 2019-10-02.
//

#ifndef ALITA_MACROS_H
#define ALITA_MACROS_H

#include <stdexcept>

#include "Platform/Platform.h"

#define NS_RHI_BEGIN namespace RHI {
#define NS_RHI_END }

#define RHI_ASSERT(cond) Assert(cond, "")

#define RHI_SAFE_RETAIN(obj)    \
do {                            \
if (obj) obj->Retain();         \
} while (0)

#define RHI_SAFE_RELEASE(obj)   \
do {                            \
    if (obj) obj->Release();    \
    (obj) = nullptr;              \
} while (0)

#define RHI_CAST(TYPE, obj) (static_cast<TYPE>(obj))

#define Assert(cond, msg) VASSERT(cond, msg)

#endif //ALITA_MACROS_H
