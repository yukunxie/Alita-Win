//
// Created by realxie on 2020-02-21.
//

#ifndef PROJ_ANDROID_ASYNCWORKER_H
#define PROJ_ANDROID_ASYNCWORKER_H

#include "Macros.h"
#include "SemaphoreCXX.h"
#include "Runnable.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include <functional>
#include <atomic>

NS_GFX_BEGIN

struct AsyncTask
{
    /**
     * Notice: 请勿在Execute中申请和释放任何资源(包括Vulkan和RHI的)。
     */
    virtual bool Execute() = 0;
    
    virtual const char* GetName() = 0;
    
    virtual ~AsyncTask()
    {}
};

typedef std::shared_ptr<AsyncTask> AsyncTaskPtr;

class AsyncWorker : public Runnable
{
public:
    explicit AsyncWorker()
        : Runnable("RHI-AsyncWorker"), semaphore_(0), isThreadStopped_(false)
    {
        Start();
    }
    
    void EnqueueInGameThread(AsyncTaskPtr pTask)
    {
        SCOPED_LOCK(mutex_);
        taskQueue_.push(pTask);
        semaphore_.Signal();
    }
    
    virtual ~AsyncWorker()
    {
        if (!IsStopped())
        {
            LOGE("~AsyncWorker(): Please stop the worker thread first.");
        }
    }
    
    void CheckThread()
    {
        if (threadId_)
        {
            GFX_ASSERT(pthread_self() == threadId_);
        }
    }
    
    bool IsStopped()
    {
        return isThreadStopped_.load(std::memory_order::memory_order_seq_cst);
    }
    
    virtual void Stop() override
    {
        LOGW("AsyncWorker stop.");
        
        if (!isThreadStopped_.exchange(true, std::memory_order::memory_order_seq_cst))
        {
            // The thread may be blocked by Dequeue().
            EnqueueInGameThread(nullptr);
            // waiting thread exit;
            Join();
        }
    
        LOGW("AsyncWorkerVulkan pending tasks: count = %d", taskQueue_.size());
    
        // Clear all pending tasks.
        while (!taskQueue_.empty())
        {
            auto task = taskQueue_.front();
            if (task)
                LOGW("AsyncWorkerVulkan pending tasks: %s", task->GetName());
            taskQueue_.pop();
            PostProcessDoneTask(task);
        }
    }

protected:
    
    AsyncTaskPtr Dequeue()
    {
        if (IsStopped())
        {
            return nullptr;
        }
        
        semaphore_.Wait();
        SCOPED_LOCK(mutex_);
        AsyncTaskPtr pTask = taskQueue_.front();
        taskQueue_.pop();
        return pTask;
    }
    
    virtual void CheckForeground()
    {};
    
    virtual void Run() override
    {
        threadId_ = pthread_self();
        
        while (!IsStopped())
        {
            CheckForeground();
            
            AsyncTaskPtr pTask = Dequeue();
            if (pTask)
            {
                // RHI_SCOPED_PROFILING_GUARD(task->GetName());
                pTask->Execute();
                PostProcessDoneTask(pTask);
            }
        }
        LOGW("AsyncWorker::Execute end.");
    }
    
    virtual void PostProcessDoneTask(AsyncTaskPtr task)
    {
        LOGE("Please stop the worker thread in derived class first.");
    }

protected:
    SemaphoreCXX semaphore_;
    std::mutex mutex_;
#if WIN32
    std::uint32_t threadId_ = 0;
#else
    std::thread::native_handle_type threadId_ = 0;
#endif
    std::queue<AsyncTaskPtr> taskQueue_;
    std::atomic<bool> isThreadStopped_;
};

NS_GFX_END

#endif //PROJ_ANDROID_ASYNCWORKER_H
