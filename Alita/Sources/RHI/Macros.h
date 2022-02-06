//
// Created by realxie on 2019-10-02.
//

#ifndef ALITA_MACROS_H
#define ALITA_MACROS_H

#include <stdexcept>
#include <functional>
#include <assert.h>

#include "Platform/Platform.h"

#if ANDROID
#include <android/log.h>
#endif

#if WIN32
#define pthread_self() (0)
#endif 

#if defined(COCOS2D_DEBUG) && COCOS2D_DEBUG
#define GFX_DEBUG 1
#else
#define GFX_DEBUG 1
#endif

#if GFX_DEBUG
#define GFX_TRACE_OBJ_REF_COUNT 0
#else
#define GFX_TRACE_OBJ_REF_COUNT 0
#endif


#define NS_GFX_BEGIN namespace gfx {
#define NS_GFX_END }

#if GFX_DEBUG
#define GFX_ASSERT(cond, ...) assert(cond)
#else
#define GFX_ASSERT(cond, ...)
#endif

#define GFX_SAFE_DELETE(obj)    \
do {                            \
if (obj) delete obj;            \
obj = nullptr;                  \
} while (0)

#if GFX_TRACE_OBJ_REF_COUNT
#define PUSH_TRACEBACK  tmp->PushTraceback(__FILE__, __FUNCTION__, __LINE__);
#define POP_TRACEBACK tmp->PopTraceback();
#else
#define PUSH_TRACEBACK
#define POP_TRACEBACK
#endif

#define GFX_SAFE_RETAIN(obj)    \
    do {                        \
    } while (0)

// #define GFX_SAFE_RETAIN(obj)    \
// do {                            \
// auto tmp = obj;                 \
// if (tmp) tmp->Retain();         \
// } while (0)


#define GFX_SAFE_RELEASE(obj)   \
do {                            \
} while (0)

#define GFX_PTR_ASSIGN(dstObjPtr, srcObjPtr)    \
do {                                            \
    auto srcObjPtr_ = (srcObjPtr);              \
    if (dstObjPtr != srcObjPtr_)                \
    {                                           \
        dstObjPtr = (decltype(dstObjPtr))srcObjPtr_;                 \
    }                                           \
} while (0)

#define GFX_CAST(TYPE, obj) (static_cast<TYPE>((obj).get()))

#define GFX_DISPOSE_BEGIN()         \
do {                                \
    if (IsDisposed())               \
        return;                     \
    SetDisposed();                  \
} while (0)

#define GFX_DISPOSE_END()                   \
do{                                         \
    GFX_ASSERT(IsDisposed());               \
} while (0)

// Android log function wrappers
static const char* kTAG = "gfx";

#if defined(_MSC_VER)
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define FORCE_INLINE inline __attribute__ ((always_inline))
#else
#define FORCE_INLINE
#endif

#define FEQUAL(x, y) (abs(x - y) < std::numeric_limits<decltype(x)>::epsilon())


#define GET_THREAD_LOCAL_ALLOCATOR() (static_cast<AsyncWorkerVulkan*>(VKDEVICE()->GetAsyncWorker())->GetLocalAllocator())

#define MACRO_COMBINE_INNER(A,B) A##B
#define MACRO_COMBINE(A,B) MACRO_COMBINE_INNER(A,B)

#define SCOPED_LOCK(mutex) std::lock_guard<std::remove_cv_t<decltype(mutex)>> MACRO_COMBINE(scroped_lock_guarder_,__LINE__)(mutex)

#ifndef Assert
#define Assert(cond, msg) VASSERT(cond, msg)
#endif

#endif //ALITA_MACROS_H
