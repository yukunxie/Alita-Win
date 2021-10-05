//
// Created by realxie on 2012/10/02.
//

#pragma once

#include "Base/ObjectBase.h"
#include "RHI/RHI.h"
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

    RenderTarget(RHI::TextureFormat format, const std::string& debugName = "");

    RenderTarget(uint32 width, uint32 height, RHI::TextureFormat format, const std::string& debugName = "");

    virtual ~RenderTarget();

    void ResizeTarget(uint32 width, uint32 height, RHI::TextureFormat format = RHI::TextureFormat::INVALID);

    virtual const RHI::TextureView* GetTextureView() const;

    virtual const RHI::Texture* GetTexture() const;

    // Getters / Setters

    float GetClearDepth() const { return ClearDepth_; }

    void SetClearDepth(float depth = 1.0f) { ClearDepth_ = depth; }

    uint8 GetClearStencil() const { return ClearStencil_; }

    void SetClearStencil(uint8 stencil) { ClearStencil_ = stencil; }

    const RHI::Color& GetClearColor() const { return ClearColor_; }

    void SetClearColor(const RHI::Color& color) { ClearColor_ = color; }

    virtual RHI::Extent3D GetExtent() const { return { TargetWidth_, TargetHeight_, 1 }; }

    virtual uint32 GetWidth() const { return TargetWidth_; }

    virtual uint32 GetHeight() const { return TargetHeight_; }

    virtual RHI::TextureFormat GetFormat() const { return Format_; }

protected:
    void TryCreateTextureView() const;

protected:
    std::string        Name_;
    mutable bool                Dirty_          = true;
    uint32              TargetWidth_    = 1;
    uint32              TargetHeight_   = 1;
    RHI::TextureFormat  Format_         = RHI::TextureFormat::RGBA8UNORM;
    uint8               ClearStencil_   = 0;
    float               ClearDepth_     = 1.0f;
    RHI::Color          ClearColor_     = { 0, 0, 0, 1 };
    mutable RHI::TextureView*   TextureView_    = nullptr;
};

class RenderTargetSwapChain : public RenderTarget
{
public:
    RenderTargetSwapChain();
    virtual ~RenderTargetSwapChain();

    virtual const RHI::TextureView* GetTextureView() const;

    virtual const RHI::Texture* GetTexture() const;

    // Getters / Setters

    virtual RHI::Extent3D GetExtent() const override;

    virtual uint32 GetWidth() const override;

    virtual uint32 GetHeight() const override;

    virtual RHI::TextureFormat GetFormat() const  override;

    void  Reset();

};


NS_RX_END
