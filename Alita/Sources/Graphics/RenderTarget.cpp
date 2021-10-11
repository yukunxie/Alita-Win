//
// Created by realxie on 2012/10/02.
//

#include "Pass.h"
#include "World/MeshComponent.h"
#include "World/World.h"
#include "World/Camera.h"
#include "RenderObject.h"
#include "Engine/Engine.h"
#include "RenderScene.h"
#include "RenderTarget.h"

NS_RX_BEGIN

RenderTarget::RenderTarget(gfx::TextureFormat format, const std::string& debugName)
    : Format_(format)
    , Name_(debugName)
{
}

RenderTarget::RenderTarget(uint32 width, uint32 height, gfx::TextureFormat format, const std::string& debugName)
    : TargetWidth_(width)
    , TargetHeight_(height)
    , Format_(format)
    , Name_(debugName)
{

}

RenderTarget::~RenderTarget()
{
    GFX_SAFE_RELEASE(TextureView_);
}

void RenderTarget::ResizeTarget(uint32 width, uint32 height, gfx::TextureFormat format)
{
    width = std::max(width, 1u);
    height = std::max(height, 1u);

    if (TargetWidth_ != width)
    {
        Dirty_ = true;
        TargetWidth_ = width;
    }

    if (TargetHeight_ != height)
    {
        Dirty_ = true;
        TargetHeight_ = height;
    }

    if (gfx::TextureFormat::INVALID != format && Format_ != format)
    {
        Dirty_ = true;
        Format_ = format;
    }
}

const gfx::TextureView* RenderTarget::GetTextureView() const
{
    TryCreateTextureView();
    return TextureView_;
}

const gfx::Texture* RenderTarget::GetTexture() const
{
    return GetTextureView()->GetTexture();
}

void RenderTarget::TryCreateTextureView() const
{
    if (!Dirty_ && TextureView_)
        return;
    Dirty_ = false;
    GFX_SAFE_RELEASE(TextureView_);

    gfx::TextureDescriptor descriptor;
    descriptor.sampleCount = 1;
    descriptor.format = Format_;
    descriptor.usage = gfx::TextureUsage::OUTPUT_ATTACHMENT | gfx::TextureUsage::SAMPLED;
    descriptor.size = {TargetWidth_,TargetHeight_, 1};
    descriptor.arrayLayerCount = 1;
    descriptor.mipLevelCount = 1;
    descriptor.dimension = gfx::TextureDimension::TEXTURE_2D;
    descriptor.debugName = Name_;
    TextureView_ = Engine::GetGPUDevice()->CreateTexture(descriptor)->CreateView({});
    GFX_SAFE_RETAIN(TextureView_);
}

RenderTargetSwapChain::RenderTargetSwapChain()
{
    Dirty_ = false;
}

RenderTargetSwapChain::~RenderTargetSwapChain()
{
}

const gfx::TextureView* RenderTargetSwapChain::GetTextureView() const
{
    return TextureView_;
}

const gfx::Texture* RenderTargetSwapChain::GetTexture() const
{
    return TextureView_->GetTexture();
}

gfx::Extent3D RenderTargetSwapChain::GetExtent() const
{
    return TextureView_->GetTexture()->GetTextureSize();
}

uint32 RenderTargetSwapChain::GetWidth() const
{
    return GetExtent().width;
}

uint32 RenderTargetSwapChain::GetHeight() const
{
    return GetExtent().height;
}

gfx::TextureFormat RenderTargetSwapChain::GetFormat() const
{
    return TextureView_->GetTexture()->GetFormat();
}
void  RenderTargetSwapChain::Reset()
{
    GFX_SAFE_RELEASE(TextureView_);
    auto swapchain = Engine::GetRenderScene()->GetGraphicPipeline()->GetSwapChain();
    TextureView_ = swapchain->GetCurrentTexture()->CreateView({});
    GFX_SAFE_RETAIN(TextureView_);
}
NS_RX_END
