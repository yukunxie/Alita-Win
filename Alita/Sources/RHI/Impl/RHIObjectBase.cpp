//
// Created by realxie on 2020/8/24.
//

#include "RHI/RHIObjectBase.h"
#include "RHI/Device.h"

NS_RHI_BEGIN

#if RHI_TRACE_OBJ_REF_COUNT

static std::vector<const std::string> sCoarseFunctionCallStack;

void RHIObjectBase::PushTraceback(const char* file, const char* funcName, std::uint32_t lineno)
{
    const static char* separator = "/cocos2d-x-lite/";
    const char* ptr = strstr(file, separator);
    ptr = ptr ? ptr + strlen(separator) : file;
    std::string info = std::string(ptr) + ":" + std::to_string(lineno) + " " + funcName;
    sCoarseFunctionCallStack.push_back(std::move(info));
}

void RHIObjectBase::PopTraceback()
{
    if (!sCoarseFunctionCallStack.empty())
    {
        sCoarseFunctionCallStack.pop_back();
    }
}

#define NEW_OBJ_REF_COUNT_TRACEBACK(tracebacks)         \
{                                                       \
    std::string traceback;                              \
    for (const auto& str : sCoarseFunctionCallStack)    \
    {                                                   \
        traceback += str + " >> ";                      \
    }                                                   \
    tracebacks.push_back(traceback);                    \
}

#else

#define NEW_OBJ_REF_COUNT_TRACEBACK(...)

#endif

#if defined(RHI_DEBUG) && RHI_DEBUG
std::thread::native_handle_type RHIObjectBase::mainThreadId_ = 0;
#endif

RHIObjectBase::~RHIObjectBase()
{
    RHI_ASSERT(globalId__ != INVALID_OBJECT_ID);
    RHI_ASSERT(bindingScriptObject_ == nullptr);
    RHI_SAFE_RELEASE(GPUDevice_);
}

RHIObjectBase::RHIObjectBase(RHIObjectType objectType)
    : objectType__(objectType)
{
    CheckThread();
    RHI_ASSERT(objectType__ != RHIObjectType::UNDEFINED);

#if AUTO_UNWIND_TRACEBACK_ENABLED
    UNWIND_CURRENT_STACK_TRACEBACK(_unwindTraceback);
#endif
}

RHIObjectBase::RHIObjectBase(Device* device, RHIObjectType objectType)
: RHIObjectBase(objectType)
{
    RHI_PTR_ASSIGN(GPUDevice_, device);
    GPUDevice_->GetObjectManager().AddObject(this);
}

RHIObjectBase* RHIObjectBase::AutoRelease()
{
    if (this->GetObjectType() == RHIObjectType::Device)
    {
        RHI_CAST(Device * , this)->AddAutoReleaseObjectToPool(this);
    }
    else
    {
        RHI_ASSERT(GPUDevice_);
        GPUDevice_->AddAutoReleaseObjectToPool(this);
    }
    
    autoReleased_ = true;
    
    return this;
}

void RHIObjectBase::Retain() const
{
    NEW_OBJ_REF_COUNT_TRACEBACK(xxxfuncCallRetainTracebacks_);
    
    CheckThread();
    RHI_ASSERT(!IsDisposed());
    RHI_ASSERT((int) objectType__ >= 0 && (int) objectType__ <= (int) RHIObjectType::MAX_COUNT);
    ++referenceCount__;
}

void RHIObjectBase::Release() const
{
    NEW_OBJ_REF_COUNT_TRACEBACK(xxxfuncCallReleaseTracebacks_);

    CheckThread();
    RHI_ASSERT(referenceCount__ > 0);
    RHI_ASSERT((int) objectType__ >= 0 && (int) objectType__ <= (int) RHIObjectType::MAX_COUNT);
    
    if (0 == --referenceCount__)
    {
        OnDeleteEvent();
        if (GPUDevice_)
        {
            GPUDevice_->GetObjectManager().RemoveObject(this);
        }
        delete this;
    }
}

NS_RHI_END

