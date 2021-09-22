//
// Created by realxie on 2020/8/26.
//

#ifndef PROJ_ANDROID_RHIOBJECTMANAGER_H
#define PROJ_ANDROID_RHIOBJECTMANAGER_H

#include <map>
#include "RHIObjectBase.h"

NS_RHI_BEGIN

class RHIObjectManager : public RHINoncopyable
{
public:
    RHIObjectManager()
        : trackedObjects_(1024 * 16, nullptr)
    {
    }
    
    RHIObjectBase* GetObject(std::uint32_t id)
    {
        if (id >= trackedObjects_.size())
        {
            return nullptr;
        }
        return trackedObjects_[id];
    }
    
    void AddObject(RHIObjectBase* objectBase);
    void RemoveObject(const RHIObjectBase* object);
    
    void AddObjectToCache(RHIObjectBase* object)
    {
        if (TestObject(object))
        {
            RHIObjectType type = object->GetObjectType();
            weakedRefObjectCache_.emplace(type, object);
        }
    }
    
    void RemoveObjectFromCache(const RHIObjectBase* object)
    {
        RHI_ASSERT(TestObject(object));
        RHIObjectType type = object->GetObjectType();
        auto itStart = weakedRefObjectCache_.lower_bound(type);
        auto itEnd = weakedRefObjectCache_.upper_bound(type);
        while (itStart != itEnd)
        {
            if (itStart->second != object)
            {
                itStart++;
                continue;
            }
            itStart = weakedRefObjectCache_.erase(itStart);
            break;
        }
    }
    
    RHIObjectBase* GetObjectFromCacheByType(RHIObjectType type)
    {
        RHIObjectBase* object = nullptr;
        auto it = weakedRefObjectCache_.find(type);
        if (it != weakedRefObjectCache_.end())
        {
            object = it->second;
            weakedRefObjectCache_.erase(it);
        }
        return object;
    }

    std::uint32_t getCommandBufferCount() { return commandBufferCount_;}
    std::uint32_t getBindGroupCount() { return bindGroupCount_; }
    std::uint32_t getRenderPipelineCount() { return renderPipelineCount_; }
    std::uint32_t getBufferCount() { return bufferCount_; }
    std::uint32_t getTextureCount() { return textureCount_; }
    void increaseTextureMem(std::uint32_t mem) { textureMemory_ += mem; }
    std::uint32_t getTextureMemory() { return textureMemory_; }
    void increaseBufferSize(std::uint32_t size) { bufferSize_ += size; }
    std::uint32_t getTotalBufferSize() { return bufferSize_; }


protected:
    bool TestObject(const RHIObjectBase* objectBase)
    {
        std::uint32_t id = objectBase->GetId();
        if (id >= trackedObjects_.size())
        {
            return false;
        }
        return trackedObjects_[id] == objectBase;
    }

protected:
    std::uint32_t dynamicRHIObjectID_ = 0;
    std::vector<RHIObjectBase*> trackedObjects_;
    std::vector<std::uint32_t> freeIds_;
    
    std::multimap<RHIObjectType, RHIObjectBase*> weakedRefObjectCache_;

private:
#if RHI_DEBUG
    void TrackObjectDebugging_(const RHIObjectBase* object);
    
    bool FindObjectDebugging_(const RHIObjectBase* object);
    
    void RemoveTrackedObjectDebugging_(const RHIObjectBase* object);
    
    // 为什么要用这个诡异的数据结构？因为可以在Android Studio上直接看到数据大小，方便问题排查
    std::uint32_t objectTrackerCount_ = 0;
    std::map<const RHIObjectBase*, RHIObjectType> objectTrackers_[kMaxRHIObjectTypeCount];
#endif

    std::uint32_t commandBufferCount_ = 0;
    std::uint32_t bindGroupCount_ = 0;
    std::uint32_t renderPipelineCount_ = 0;
    std::uint32_t bufferCount_ = 0;
    std::uint32_t bufferSize_ = 0;
    std::uint32_t textureCount_ = 0;
    std::uint32_t textureMemory_ = 0;
};

NS_RHI_END

#endif //PROJ_ANDROID_RHIOBJECTMANAGER_H
