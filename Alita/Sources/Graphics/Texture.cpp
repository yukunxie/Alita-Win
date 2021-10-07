//
// Created by realxie on 2012/10/06.
//

#include "Texture.h"
#include "Base/FileSystem.h"
#include "Engine/Engine.h"
#include "Loaders/ImageLoader.h"


NS_RX_BEGIN

// static 
std::shared_ptr<Texture> Texture::LoadTextureFromUri(const std::string& filename)
{
    return std::make_shared<Texture>(ImageLoader::LoadTextureFromUri(filename));
}

std::shared_ptr<Texture> Texture::LoadCubeTexture(const std::string& cubeTextureName)
{
    return std::make_shared<Texture>(ImageLoader::LoadCubeTexture(cubeTextureName));
}

std::shared_ptr<Texture> Texture::LoadTextureFromData(const std::uint8_t* data, std::uint32_t byteLength, const std::string& debugName)
{
    return std::make_shared<Texture>(ImageLoader::LoadTextureFromData(data, byteLength, debugName));
}

std::shared_ptr<Texture> Texture::LoadTextureFromData(uint32 width, uint32 height, uint32 component, const uint8* data, uint32 byteLength, const std::string& debugName)
{
    return std::make_shared<Texture>(ImageLoader::LoadTextureFromData(width, height, component, data, byteLength, debugName));
}

Texture::Texture(RHI::Texture* texture)
    : Resource(EResourceType::Texture)
{
    RHI_SAFE_RELEASE(Texture_);
    Texture_ = texture;
    RHI_SAFE_RETAIN(Texture_);
}

Texture::Texture(uint32 width, uint32 height, RHI::TextureFormat format, const RHI::Color& color)
    : Resource(EResourceType::Texture)
{
    RHI_ASSERT(false);
}

Texture::~Texture()
{
    RHI_SAFE_RELEASE(TextureView_);
    RHI_SAFE_RELEASE(Texture_);
}

const RHI::TextureView* Texture::GetTextureView() const
{
    if (TextureView_)
    {
        return TextureView_;
    }
    TextureView_ = Texture_->CreateView({});
    RHI_SAFE_RETAIN(TextureView_);
    return TextureView_;
}

const RHI::Texture* Texture::GetTexture() const
{
    return Texture_;
}

// static 


NS_RX_END
