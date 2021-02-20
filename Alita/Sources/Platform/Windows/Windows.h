#pragma once

#define GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>

enum LogLevel
{
    Verbose = 0,
    Info,
    Debug,
    Warning,
    Error,
};

void _OutputLog(LogLevel level, const char* format, ...);

#define LOGI(format,...) _OutputLog(LogLevel::Info, format, ##__VA_ARGS__);
  
#define LOGW(format,...) _OutputLog(LogLevel::Warning, format, ##__VA_ARGS__);
  
#define LOGE(format,...) _OutputLog(LogLevel::Error, format, ##__VA_ARGS__);

  
#ifndef DEBUG
#define VASSERT(expr, expr_desc) ((void)0)
#else
#define VASSERT(expr, expr_desc) \
    do { if (!(expr) && \
    (1 == _CrtDbgReport(_CRT_ASSERT, (__FILE__), __LINE__, \
    NULL, #expr##"\nProblem: "##expr_desc))) \
    _CrtDbgBreak();}while(0)
#endif
  