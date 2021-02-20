
// global namespace, rx for RealXie

#ifdef ANDROID
#include <android/log.h>
#endif

#include "Platform/Platform.h"

#define NS_RX_BEGIN namespace rx{

#define NS_RX_END  }

#define NS_RX rx

#define RX_SAFE_RETAIN(obj) do {if(obj)obj->Retain();} while(0)

#define RX_SAFE_RELEASE(obj) do {if(obj)(obj)->Release(); (obj) = nullptr;} while(0)

#define RX_ASSERT(cond) Assert(cond, "")


#ifdef ANDROID

#ifndef kTAG
// Android log function wrappers
#define kTAG "RX";

#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
  
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
  
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))
  
#endif
  
#else

#define LOGI(...)

#define LOGW(...)

#define LOGE(...)

#endif