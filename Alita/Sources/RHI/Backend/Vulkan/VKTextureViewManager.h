//
// Created by realxie on 2020/8/27.
//

#ifndef PROJ_ANDROID_VKTEXTUREVIEWMANAGER_H
#define PROJ_ANDROID_VKTEXTUREVIEWMANAGER_H

#include "VKDevice.h"
#include "VKTexture.h"
#include "VKTextureView.h"

NS_GFX_BEGIN

class VKTextureViewManager
{
public:
    VKTextureViewManager(VKDevice* device)
    {
        device_ = device;
    }
    
    ~VKTextureViewManager()
    {
        Purge();
    }
    
    void Purge()
    {
        LOGI("Purge VKTextureViewManager");
        std::vector<VKTextureView*> pendingReleasedTVs;
        for (auto &it : cache_)
        {
            pendingReleasedTVs.push_back(it.second.textureView);
        }
        cache_.clear();
        
        for (auto tv : pendingReleasedTVs)
        {
            RHI_SAFE_RELEASE(tv);
        }
        
        LOGI("Purge VKTextureViewManager done.");
    }
    
    void RemoveByTexture(Texture* texture)
    {
        std::vector<VKTextureView*> pendingReleasedTVs;
        
        auto itStart = cache_.lower_bound(texture);
        auto itEnd = cache_.upper_bound(texture);
        for (auto it = itStart; it != itEnd; ++it)
        {
            pendingReleasedTVs.push_back(it->second.textureView);
        }
        cache_.erase(itStart, itEnd);
        
        for (auto tv : pendingReleasedTVs)
        {
            RHI_SAFE_RELEASE(tv);
        }
    }
    
    VKTextureView* GetOrCreateTextureView(Texture* texture, const TextureViewDescriptor &descriptor)
    {
        auto it = cache_.lower_bound(texture);
        for (auto itEnd = cache_.upper_bound(texture); it != itEnd; ++it)
        {
            if (it->second.descriptor == descriptor)
            {
                return it->second.textureView;
            }
        }
        
        auto vkTexture = RHI_CAST(VKTexture*, texture);
        auto texView = device_->CreateObject<VKTextureView>(vkTexture, descriptor);
        RHI_SAFE_RETAIN(texView);
        cache_.emplace(texture, TextureViewCacheItem{texView, descriptor});
        
        return texView;
    }
    
    std::vector<VKTextureView*> GetAllCreatedTextureViews(Texture* texture)
    {
        std::vector<VKTextureView*> tvs;
        auto it = cache_.lower_bound(texture);
        for (auto itEnd = cache_.upper_bound(texture); it != itEnd; ++it)
        {
            tvs.push_back(it->second.textureView);
        }
        return std::move(tvs);
    }

protected:
    VKDevice* device_ = nullptr;
    
    struct TextureViewCacheItem
    {
        VKTextureView* textureView = nullptr;
        TextureViewDescriptor descriptor;
    };
    std::multimap<Texture*, TextureViewCacheItem> cache_;
};

NS_GFX_END

#endif //PROJ_ANDROID_VKTEXTUREVIEWMANAGER_H
