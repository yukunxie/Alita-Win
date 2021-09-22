//
// Created by realxie on 2020/8/28.
//

#include "RHI/Buffer.h"
#include "RHI/Texture.h"
#include "RHI/RHIObjectManager.h"

NS_RHI_BEGIN


void RHIObjectManager::AddObject(RHIObjectBase* objectBase)
{
    if (TestObject(objectBase))
    {
        return;
    }

    std::uint32_t id = objectBase->GetId();
    do
    {
        if (id != INVALID_OBJECT_ID)
        {
            LOGE("RHIObjectManager-AddObject, Invalid id");
        }

        if (!freeIds_.empty())
        {
            id = freeIds_.back();
            freeIds_.pop_back();
            break;
        }

        id = dynamicRHIObjectID_++;
    } while (0);

    objectBase->SetId(id);

    if (id >= trackedObjects_.size())
    {
        trackedObjects_.resize(std::max(trackedObjects_.size() * 2, (size_t) 1024), nullptr);
    }
    trackedObjects_[id] = objectBase;

    if (objectBase->GetObjectType() == RHIObjectType::CommandBuffer)
    {
        commandBufferCount_++;
    }
    else if (objectBase->GetObjectType() == RHIObjectType::BindGroup)
    {
        bindGroupCount_++;
    }
    else if (objectBase->GetObjectType() == RHIObjectType::RenderPipeline)
    {
        renderPipelineCount_++;
    }
    else if (objectBase->GetObjectType() == RHIObjectType::Buffer)
    {
        bufferCount_++;
    }
    else if (objectBase->GetObjectType() == RHIObjectType::Texture)
    {
        textureCount_++;
    }

#if RHI_DEBUG
    TrackObjectDebugging_(objectBase);
#endif
}
    
void RHIObjectManager::RemoveObject(const RHIObjectBase* object)
{
    if (object->GetObjectType() == RHIObjectType::CommandBuffer)
    {
        commandBufferCount_--;
    }
    else if (object->GetObjectType() == RHIObjectType::BindGroup)
    {
        bindGroupCount_--;
    }
    else if (object->GetObjectType() == RHIObjectType::RenderPipeline)
    {
        renderPipelineCount_--;
    }
    else if (object->GetObjectType() == RHIObjectType::Buffer)
    {
        bufferCount_--;

        RHIObjectBase* baseObj = const_cast<RHIObjectBase*>(object);
        auto buffer = RHI_CAST(Buffer*, baseObj);
        bufferSize_ -= buffer->GetBufferSize();
    }
    else if (object->GetObjectType() == RHIObjectType::Texture)
    {
        textureCount_--;

        RHIObjectBase* baseObj = const_cast<RHIObjectBase*>(object);
        auto tex = RHI_CAST(Texture*, baseObj);
        textureMemory_ -= tex->getMemoryUsage();
    }

    RemoveObjectFromCache(object);

    std::uint32_t id = object->GetId();
    if (id == INVALID_OBJECT_ID)
    {
        LOGE("Invalid ObjectId");
        return;
    }

    if (id >= trackedObjects_.size())
    {
        LOGE("Invalid ObjectId - overflow");
        return;
    }

    if (!trackedObjects_[id])
    {
        LOGE("Invalid ObjectId - not tracked");
        return;
    }

    if (trackedObjects_[id] != object)
    {
        LOGE("Invalid ObjectId - not equal");
        return;
    }

    trackedObjects_[id] = nullptr;
    freeIds_.push_back(id);

#if RHI_DEBUG
    RemoveTrackedObjectDebugging_(object);
#endif
}

    
#if RHI_DEBUG
void RHIObjectManager::TrackObjectDebugging_(const RHIObjectBase* object)
{
    auto type = (std::uint32_t) object->GetObjectType();
    RHI_ASSERT(type > 0 && type < kMaxRHIObjectTypeCount);
    objectTrackers_[type][object] = object->GetObjectType();
    objectTrackerCount_++;
}

bool RHIObjectManager::FindObjectDebugging_(const RHIObjectBase* object)
{
    for (std::uint32_t i = 0; i < kMaxRHIObjectTypeCount; ++i)
    {
        if (objectTrackers_[i].find(object) != objectTrackers_[i].end())
        {
            return true;
        }
    }
    return false;
}

void RHIObjectManager::RemoveTrackedObjectDebugging_(const RHIObjectBase* object)
{
    if (!FindObjectDebugging_(object))
    {
        return;
    }
    
    auto type = (std::uint32_t) object->GetObjectType();
    RHI_ASSERT(type > 0 && type < kMaxRHIObjectTypeCount);
    auto &objects = objectTrackers_[type];
    
    auto it = objects.find(object);
    RHI_ASSERT(it != objects.end());
    if (it != objects.end())
    {
        objects.erase(it);
    }
    objectTrackerCount_--;
}
#endif // #if RHI_DEBUG

NS_RHI_END

