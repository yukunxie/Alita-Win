//
// Created by realxie on 2012/10/02.
//

#pragma once

#include "Base/ObjectBase.h"
#include "GFX/GFX.h"
#include "Types/Types.h"
#include "World/Camera.h"
#include "Technique.h"

NS_RX_BEGIN

class RenderTarget
{
public:
    RenderTarget(const std::string& debugName = "")
        : Name_(debugName)
    {
    }

    RenderTarget(gfx::TextureFormat format, const std::string& debugName = "");

    RenderTarget(uint32 width, uint32 height, gfx::TextureFormat format, const std::string& debugName = "");

    virtual ~RenderTarget();

    void ResizeTarget(uint32 width, uint32 height, gfx::TextureFormat format = gfx::TextureFormat::INVALID);

    virtual const gfx::TextureView* GetTextureView() const;

    virtual const gfx::Texture* GetTexture() const;

    // Getters / Setters

    float GetClearDepth() const { return ClearDepth_; }

    void SetClearDepth(float depth = 1.0f) { ClearDepth_ = depth; }

    uint8 GetClearStencil() const { return ClearStencil_; }

    void SetClearStencil(uint8 stencil) { ClearStencil_ = stencil; }

    const gfx::Color& GetClearColor() const { return ClearColor_; }

    void SetClearColor(const gfx::Color& color) { ClearColor_ = color; }

    virtual gfx::Extent3D GetExtent() const { return { TargetWidth_, TargetHeight_, 1 }; }

    virtual uint32 GetWidth() const { return TargetWidth_; }

    virtual uint32 GetHeight() const { return TargetHeight_; }

    virtual gfx::TextureFormat GetFormat() const { return Format_; }

protected:
    void TryCreateTextureView() const;

protected:
    std::string        Name_;
    mutable bool                Dirty_          = true;
    uint32              TargetWidth_    = 1;
    uint32              TargetHeight_   = 1;
    gfx::TextureFormat  Format_         = gfx::TextureFormat::RGBA8UNORM;
    uint8               ClearStencil_   = 0;
    float               ClearDepth_     = 1.0f;
    gfx::Color          ClearColor_     = { 0, 0, 0, 1 };
    mutable gfx::TextureView*   TextureView_    = nullptr;
};

class RenderTargetSwapChain : public RenderTarget
{
public:
    RenderTargetSwapChain();
    virtual ~RenderTargetSwapChain();

    virtual const gfx::TextureView* GetTextureView() const;

    virtual const gfx::Texture* GetTexture() const;

    // Getters / Setters

    virtual gfx::Extent3D GetExtent() const override;

    virtual uint32 GetWidth() const override;

    virtual uint32 GetHeight() const override;

    virtual gfx::TextureFormat GetFormat() const  override;

    void  Reset();

};


NS_RX_END
