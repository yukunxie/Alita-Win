//
// Created by realxie on 2020-04-09.
//

#ifndef PROJ_ANDROID_QUERYSET_H
#define PROJ_ANDROID_QUERYSET_H

#include "GFXBase.h"
#include "Descriptors.h"

NS_GFX_BEGIN

class QuerySet : public GfxBase
{
public:
    QuerySet(Device* GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::QuerySet)
    {}
    
    bool Init(const QuerySetDescriptor &descriptor)
    {
        queryType_ = descriptor.type;
        count_ = descriptor.count;
        return true;
    }
    
    QueryType GetQueryType() const
    { return queryType_; }
    
    std::uint32_t GetCount() const
    { return count_; }

protected:
    virtual ~QuerySet() = default;

protected:
    QueryType queryType_ = QueryType::OCCLUSION;
    std::uint32_t count_ = 0;
};

NS_GFX_END

#endif //PROJ_ANDROID_QUERYSET_H
