//
// Created by realxie on 2012/10/02.
//

#pragma once

#include "Base/ObjectBase.h"
#include "Base/Resource.h"
#include "RHI/RHI.h"
#include <memory>
#include <string>

NS_RX_BEGIN

class Texture : public Resource
{
public:
    Texture(RHI::Texture* texture);

    Texture(uint32 width, uint32 height, RHI::TextureFormat format, const RHI::Color& color);

    virtual ~Texture();

    virtual const RHI::TextureView* GetTextureView() const;

    virtual const RHI::Texture* GetTexture() const;

    virtual RHI::Extent3D GetExtent() const { return { Width_, Height_, 1 }; }

    virtual uint32 GetWidth() const { return Width_; }

    virtual uint32 GetHeight() const { return Height_; }

    virtual RHI::TextureFormat GetFormat() const { return Format_; }

    void SetDebugName(const std::string& debugName) { Name_ = debugName; }

public:
    static std::shared_ptr<Texture> LoadTextureFromUri(const std::string& filename);

    static std::shared_ptr<Texture> LoadCubeTexture(const std::string& cubeTextureName);

    static std::shared_ptr<Texture> LoadTextureFromData(const std::uint8_t* data, std::uint32_t byteLength, const std::string& debugName = "");

    static std::shared_ptr<Texture> LoadTextureFromData(uint32 width, uint32 height, uint32 component, const uint8* data, uint32 byteLength, const std::string& debugName = "");

protected:
    std::string                 Name_;
    uint32                      Width_          = 1;
    uint32                      Height_         = 1;
    RHI::TextureFormat          Format_         = RHI::TextureFormat::RGBA8UNORM;
    RHI::Texture*               Texture_        = nullptr;
    mutable RHI::TextureView*   TextureView_    = nullptr;
};
NS_RX_END
