#pragma once

#ifdef WIN32
#include "Windows/Windows.h"
#elif defined(Android)
#include "Android/Android.h"
#endif

void AssertImpl(const char* filename, const char* function, int lineno, const char* format, ...);