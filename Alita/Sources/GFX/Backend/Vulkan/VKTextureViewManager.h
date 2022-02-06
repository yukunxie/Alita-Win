//
// Created by realxie on 2020/8/27.
//

#ifndef PROJ_ANDROID_VKTEXTUREVIEWMANAGER_H
#define PROJ_ANDROID_VKTEXTUREVIEWMANAGER_H

#include "VKDevice.h"
#include "VKTexture.h"
#include "VKTextureView.h"

NS_GFX_BEGIN

class TextureViewManager
{
public:
    TextureViewManager(const DevicePtr& device)
    {
        device_ = device;
    }
    
    ~TextureViewManager()
    {
        Purge();
    }
    
    void Purge()
    {
        LOGI("Purge TextureViewManager");

        cache_.clear();

        LOGI("Purge TextureViewManager done.");
    }
    
    void RemoveByTexture(Texture* texture)
    {
        auto itStart = cache_.lower_bound(texture);
        auto itEnd = cache_.upper_bound(texture);
        cache_.erase(itStart, itEnd);
    }
    
    TextureViewPtr GetOrCreateTextureView(const TexturePtr& texture, const TextureViewDescriptor &descriptor)
    {
        auto it = cache_.lower_bound(texture.get());
        for (auto itEnd = cache_.upper_bound(texture.get()); it != itEnd; ++it)
        {
            if (it->second.descriptor == descriptor)
            {
                return it->second.textureView;
            }
        }
        
        auto texView = GFX_CAST(VKDevice*, device_)->CreateObject<TextureViewPtr, VKTextureView>(texture, descriptor);
        cache_.emplace(texture.get(), TextureViewCacheItem{texView, descriptor});
        
        return texView;
    }
    
    std::vector<TextureViewPtr> GetAllCreatedTextureViews(Texture* texture)
    {
        std::vector<TextureViewPtr> tvs;
        auto it = cache_.lower_bound(texture);
        for (auto itEnd = cache_.upper_bound(texture); it != itEnd; ++it)
        {
            tvs.push_back(it->second.textureView);
        }
        return std::move(tvs);
    }

protected:
    DevicePtr device_ = nullptr;
    
    struct TextureViewCacheItem
    {
        TextureViewPtr textureView = nullptr;
        TextureViewDescriptor descriptor;
    };
    std::multimap<Texture*, TextureViewCacheItem> cache_;
};

NS_GFX_END

#endif //PROJ_ANDROID_VKTEXTUREVIEWMANAGER_H
