//
// Created by realxie on 2020/8/26.
//

#pragma once

#include <map>
#include "GFXBase.h"

NS_GFX_BEGIN

class GfxObjectManager : public GfxNoncopyable
{
public:
    GfxObjectManager()
        : trackedObjects_(1024 * 16, nullptr)
    {
    }
    
    GfxBase* GetObject(std::uint32_t id)
    {
        if (id >= trackedObjects_.size())
        {
            return nullptr;
        }
        return trackedObjects_[id];
    }
    
    void AddObject(GfxBase* objectBase);
    void RemoveObject(const GfxBase* object);
    
    void AddObjectToCache(GfxBase* object)
    {
        if (TestObject(object))
        {
            RHIObjectType type = object->GetObjectType();
            weakedRefObjectCache_.emplace(type, object);
        }
    }
    
    void RemoveObjectFromCache(const GfxBase* object)
    {
        GFX_ASSERT(TestObject(object));
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
    
    GfxBase* GetObjectFromCacheByType(RHIObjectType type)
    {
        GfxBase* object = nullptr;
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
    bool TestObject(const GfxBase* objectBase)
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
    std::vector<GfxBase*> trackedObjects_;
    std::vector<std::uint32_t> freeIds_;
    
    std::multimap<RHIObjectType, GfxBase*> weakedRefObjectCache_;

private:
#if GFX_DEBUG
    void TrackObjectDebugging_(const GfxBase* object);
    
    bool FindObjectDebugging_(const GfxBase* object);
    
    void RemoveTrackedObjectDebugging_(const GfxBase* object);
    
    // 为什么要用这个诡异的数据结构？因为可以在Android Studio上直接看到数据大小，方便问题排查
    std::uint32_t objectTrackerCount_ = 0;
    std::map<const GfxBase*, RHIObjectType> objectTrackers_[kMaxRHIObjectTypeCount];
#endif

    std::uint32_t commandBufferCount_ = 0;
    std::uint32_t bindGroupCount_ = 0;
    std::uint32_t renderPipelineCount_ = 0;
    std::uint32_t bufferCount_ = 0;
    std::uint32_t bufferSize_ = 0;
    std::uint32_t textureCount_ = 0;
    std::uint32_t textureMemory_ = 0;
};

NS_GFX_END
