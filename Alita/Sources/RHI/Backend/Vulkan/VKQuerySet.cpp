//
// Created by realxie on 2020-04-09.
//

#include "VKQuerySet.h"

NS_RHI_BEGIN


VKQuerySet::VKQuerySet(VKDevice* device)
    : QuerySet(device)
{
}

void VKQuerySet::Dispose()
{
    RHI_DISPOSE_BEGIN();
    
    if (vkQueryPool_ != VK_NULL_HANDLE)
    {
        vkDestroyQueryPool(VKDEVICE()->GetNative(), vkQueryPool_, nullptr);
        vkQueryPool_ = VK_NULL_HANDLE;
    }
    
    RHI_DISPOSE_END();
}

VKQuerySet::~VKQuerySet()
{
    Dispose();
}

bool VKQuerySet::Init(const QuerySetDescriptor &descriptor)
{
    QuerySet::Init(descriptor);
    
    VkQueryPoolCreateInfo createInfo = {};
    {
        createInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        createInfo.queryType = VK_QUERY_TYPE_OCCLUSION;
        createInfo.queryCount = descriptor.count;
        createInfo.pipelineStatistics = VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;
    }
    
    CALL_VK(vkCreateQueryPool(VKDEVICE()->GetNative(), &createInfo, nullptr, &vkQueryPool_));
    
    return VK_NULL_HANDLE != vkQueryPool_;
}

NS_RHI_END
