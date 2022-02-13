//
// Created by realxie on 2019-12-30.
//

#ifndef RHI_VKDEVICE_HELPER_H
#define RHI_VKDEVICE_HELPER_H

#include "GFX/GFX.h"
#include "GFX/xxhash64.h"

#include "VulkanMacros.h"
#include "VKTypes.h"

#if ANDROID
#include "vulkan_wrapper.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_android.h>
#include <android/native_window.h>
#endif

#include <vector>
#include <map>
#include <unordered_map>
#include <bitset>
#include <array>
#include <functional>

NS_GFX_BEGIN

class VKFramebuffer;

class VKRenderPass;

class VKTextureView;

class VKDevice;

struct RenderPassCacheQuery
{
    RenderPassCacheQuery()
    {
        // For hashing
        memset(this, 0, sizeof(RenderPassCacheQuery));
    }
    
    RenderPassCacheQuery(const RenderPassCacheQuery& other)
    {
        memcpy(this, &other, sizeof(*this));
    }
    
    std::uint32_t GetColorAttachmentCount() const
    {
        return kMaxColorAttachments;
    }
    
    bool TestColorAttachment(std::uint32_t index) const
    {
        return colorMask & (1 << index);
    }
    
    bool TestResolvedColorAttachment(std::uint32_t index) const
    {
        return bHasResolveAttachments[index];
    }
    
    void SetColor(uint32_t index, TextureFormat format, LoadOp loadOp, StoreOp storeOp, bool hasResolveAttachment, std::uint32_t sampleCount)
    {
        colorMask |= 1 << index;
        colorFormats[index] = format;
        colorLoadOps[index] = loadOp;
        colorStoreOps[index] = storeOp;
        sampleCounts[index] = sampleCount;
        bHasResolveAttachments[index] = hasResolveAttachment;
    }
    
    void SetDepthStencil(TextureFormat format, LoadOp depthLoadOp, StoreOp depthStoreOp, LoadOp stencilLoadOp, StoreOp stencilStoreOp)
    {
        this->bHasDepthStencil = 1;
        this->depthStencilFormat = format;
        this->depthLoadOp = depthLoadOp;
        this->depthStoreOp = depthStoreOp;
        this->stencilLoadOp = stencilLoadOp;
        this->stencilStoreOp = stencilStoreOp;
    }
    
    bool operator==(const RenderPassCacheQuery &other) const
    {
        return !memcmp(this, &other, sizeof(*this));
    }
    
    size_t operator()() const
    {
        static XXHash64 _hashFunc(0x21378732);
        return (size_t) (_hashFunc.hash(this, sizeof(*this), 0));
    }
    
    std::uint32_t colorMask = 0;
    TextureFormat colorFormats[kMaxColorAttachments];
    LoadOp colorLoadOps[kMaxColorAttachments];
    StoreOp colorStoreOps[kMaxColorAttachments];
    std::uint8_t sampleCounts[kMaxColorAttachments];
    std::uint8_t bHasResolveAttachments[kMaxColorAttachments];
    std::uint8_t bIsSwapchainTextures[kMaxColorAttachments];
    std::uint8_t bHasDepthStencil;
    TextureFormat depthStencilFormat;
    LoadOp depthLoadOp;
    StoreOp depthStoreOp;
    LoadOp stencilLoadOp;
    StoreOp stencilStoreOp;
};

struct RenderPassCacheQueryFuncs
{
    size_t operator()(const RenderPassCacheQuery &query) const
    {
        return query();
    }
    
    bool operator()(const RenderPassCacheQuery &a, const RenderPassCacheQuery &b) const
    {
        return a == b;
    }
};

typedef std::unordered_map<RenderPassCacheQuery, RenderPassPtr, RenderPassCacheQueryFuncs, RenderPassCacheQueryFuncs> RenderPassCache;

struct FramebufferCacheQuery
{
    FramebufferCacheQuery()
    {
        memset(this, 0, sizeof(*this));
        layers = 1;
    }
    
    FramebufferCacheQuery(const FramebufferCacheQuery& other)
    {
        memcpy(this, &other, sizeof(*this));
    }
    
    bool operator==(const FramebufferCacheQuery &other) const
    {
        int ret = memcmp(this, &other, sizeof(FramebufferCacheQuery));
        return ret == 0;
    }
    
    size_t operator()() const
    {
        static XXHash64 _hashFunc(0x21378732);
        return (size_t) (_hashFunc.hash(&renderPass, sizeof(FramebufferCacheQuery), 0));
    }
    
    RenderPassPtr renderPass;
    TextureViewPtr attachments[kMaxColorAttachments];
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t layers = 1;
};

struct FramebufferCacheQueryFuncs
{
    size_t operator()(const FramebufferCacheQuery &query) const
    {
        return query();
    }
    
    bool operator()(const FramebufferCacheQuery &a, const FramebufferCacheQuery &b) const
    {
        return a == b;
    }
};

typedef std::unordered_map<FramebufferCacheQuery, FramebufferPtr, FramebufferCacheQueryFuncs, FramebufferCacheQueryFuncs> FramebufferCache;

NS_GFX_END

#endif //RHI_VKDEVICE_HELPER_H
