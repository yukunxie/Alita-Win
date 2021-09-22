//
// Created by realxie on 2020-03-23.
//

#ifndef PROJ_ANDROID_RUNNABLE_H
#define PROJ_ANDROID_RUNNABLE_H

#include "Macros.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include <functional>
#include <atomic>
#include <string>

NS_RHI_BEGIN

class Runnable
{
public:
    Runnable(const char* name)
        : threadName_(name)
    {
    }
    
    Runnable(const std::string &name)
        : threadName_(name)
    {
    }
    
    Runnable(std::string &&name)
        : threadName_(std::forward<std::string&&>(name))
    {
    }
    
    Runnable* Start()
    {
        if (bHasStarted_)
        {
            return this;
        }
        bHasStarted_ = 1;
        runningThread_ = new std::thread(std::bind(&Runnable::Execute, this));
        return this;
    }
    
    virtual void Stop() {}
    
    virtual ~Runnable()
    {
        Join();
    }
    
protected:
    
    void Execute()
    {
#if ANDROID
        pthread_setname_np(pthread_self(), threadName_.c_str());
#endif
        Run();
    }
    
    virtual void Run() = 0;
    
    void Join()
    {
        if (runningThread_)
        {
            runningThread_->join();
            LOGW("Runnable tread joined");
            delete runningThread_;
            runningThread_ = nullptr;
        }
    }

private:
    std::string threadName_;
    std::uint32_t bHasStarted_ = 0;
    std::thread* runningThread_ = nullptr;
};

NS_RHI_END

#endif //PROJ_ANDROID_RUNNABLE_H
