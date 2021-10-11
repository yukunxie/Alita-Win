//
// Created by realxie on 2020-03-04.
//

#ifndef PROJ_ANDROID_TIMEPROFILER_H
#define PROJ_ANDROID_TIMEPROFILER_H

#include "Macros.h"
#include <stdio.h>
#include <chrono>

NS_GFX_BEGIN

class TimeProfiler
{
public:
    TimeProfiler() = delete;
    
    explicit TimeProfiler(const char* tag)
    {
        tag_ = tag;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~TimeProfiler()
    {
        auto nowTime = std::chrono::high_resolution_clock::now();
        long duration = static_cast<long>(std::chrono::duration_cast<std::chrono::microseconds>(
            nowTime - startTime_).count());
        LOGI("@@@@TimeProfiler: %s : %.03fms", tag_.c_str(), duration/1000.0f);
    }

protected:
    std::string tag_;
    std::chrono::steady_clock::time_point startTime_;
};

#define RHI_SCOPED_PROFILING_GUARD(name) TimeProfiler timeProfiler##__LINE__(name)

NS_GFX_END

#endif //PROJ_ANDROID_TIMEPROFILER_H
