//
// Created by realxie on 2020-02-21.
//

#ifndef PROJ_ANDROID_SEMAPHORECXX_H
#define PROJ_ANDROID_SEMAPHORECXX_H

#include "Macros.h"

#include <condition_variable>
#include <mutex>
#include <thread>

NS_GFX_BEGIN

class SemaphoreCXX
{
public:
    explicit SemaphoreCXX(int count = 0) : count_(count)
    {
    }
    
    void Signal()
    {
        std::unique_lock <std::mutex> lock(mutex_);
        ++count_;
        cv_.notify_one();
    }
    
    void Wait()
    {
        std::unique_lock <std::mutex> lock(mutex_);
        cv_.wait(lock, [=] { return count_ > 0; });
        --count_;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};

NS_GFX_END

#endif //PROJ_ANDROID_SEMAPHORECXX_H
