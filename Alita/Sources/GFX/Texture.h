//
// Created by realxie on 2019-10-07.
//

#ifndef RHI_TEXTURE_H
#define RHI_TEXTURE_H

#include "Macros.h"
#include "GFXBase.h"
#include "Descriptors.h"

NS_GFX_BEGIN

class TextureView;

class Texture : public GfxBase
{
public:
    Texture(DevicePtr GPUDevice)
        : GfxBase(GPUDevice, RHIObjectType::Texture)
    {
    }
    
    bool Init(const TextureDescriptor &descriptor)
    {
        textureFormat_ = descriptor.format;
        textureUsage_ = descriptor.usage;
        mipLevelCount_ = descriptor.mipLevelCount;
        arrayLayerCount_ = descriptor.arrayLayerCount;
        textureDimension_ = descriptor.dimension;
        sampleCount_ = descriptor.sampleCount;
        textureSize_ = descriptor.size;
        return true;
    }
    
    TextureFormat GetFormat() const
    {
        return textureFormat_;
    }
    
    Extent3D GetTextureSize() const
    {
        return textureSize_;
    }
    
    std::uint32_t GetSampleCount() const
    {
        return sampleCount_;
    }
    
    TextureUsageFlags GetTextureUsage() const
    {
        return textureUsage_;
    }
    
    bool TestTextureUsage(std::uint32_t usage)
    {
        return textureUsage_ & usage;
    }
    
    std::uint32_t GetArrayLayerCount() const
    {
        return arrayLayerCount_;
    }
    
    std::uint32_t GetMipLevelCount() const
    {
        return mipLevelCount_;
    }
    
    TextureDimension GetImageType() const
    {
        return textureDimension_;
    }
    
    bool Is3DImage() const
    {
        return textureDimension_ == TextureDimension::TEXTURE_3D;
    }
    
    bool Is2DImage() const
    {
        return textureDimension_ == TextureDimension::TEXTURE_2D;
    }
    
    bool Is1DImage() const
    {
        return textureDimension_ == TextureDimension::TEXTURE_1D;
    }
    
    bool IsDepthStencilFormat()
    {
        return textureFormat_ == TextureFormat::DEPTH24PLUS_STENCIL8 ||
            textureFormat_ == TextureFormat::DEPTH24PLUS ||
            textureFormat_ == TextureFormat::DEPTH32FLOAT;
    }
    
    bool IsSwapchainImage() const
    { return isSwapchainImage_; }
    
    void MarkSwapchainImage()
    { isSwapchainImage_ = true; }

    std::uint32_t getMemoryUsage()
    { return memoryUsage_; }

    virtual ~Texture() = default;

protected:
    TextureFormat textureFormat_;
    Extent3D textureSize_;
    mutable TextureUsageFlags textureUsage_ = TextureUsage::UNDEFINED;
    std::uint32_t arrayLayerCount_ = 1;
    std::uint32_t mipLevelCount_ = 1;
    std::uint32_t sampleCount_ = 1;
    TextureDimension textureDimension_ = TextureDimension::TEXTURE_2D;
    bool isSwapchainImage_ = false;

    std::uint32_t memoryUsage_ = 0;
};

NS_GFX_END

#endif //RHI_TEXTURE_H
