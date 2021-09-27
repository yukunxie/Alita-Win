//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_PASS_H
#define ALITA_PASS_H

#include "Base/ObjectBase.h"
#include "RHI/RHI.h"
#include "Types/Types.h"
#include "World/Camera.h"
#include "Technique.h"

NS_RX_BEGIN

enum class PassType
{
    OPAQUE,
    TRANSPARENT,
    UI,
};

class MeshComponent;

class RenderObject;

struct AttachmentConfig
{
    const RHI::TextureView* RenderTarget= nullptr;
    uint8               Slot            = 0xFF;
    bool                Clear           = true;
    uint8               ClearStencil    = 0;
    float               ClearDepth      = 1.0f;
    RHI::Color          ClearColor      = { 0, 0, 0, 1 };
    
};

class Pass : public ObjectBase
{
public:
    void SetupDepthStencilAttachemnt(const RHI::TextureView* attachment, bool clear = true, float depth = 1.0f, uint8 stencil = 0)
    {
        DepthStencilAttachment_.RenderTarget = attachment;
        DepthStencilAttachment_.Clear = clear;
        DepthStencilAttachment_.ClearDepth = depth;
        DepthStencilAttachment_.ClearStencil = stencil;
    }

    void SetupOutputAttachment(std::uint32_t index, const RHI::TextureView* attachment, bool clear = true, const RHI::Color& clearColor = { 0, 0, 0, 1})
    {
        attachments_.push_back(AttachmentConfig
            {
                .RenderTarget = attachment,
                .Slot = (uint8)index,
                .Clear = clear,
                .ClearStencil = 0,
                .ClearDepth = 1.0f,
                .ClearColor = clearColor
            });
    }

    void Reset()
    {
        RenderPassEncoder_ = nullptr;
        attachments_.clear();
        DepthStencilAttachment_.RenderTarget = nullptr;
    }

    void BeginPass();

    void EndPass();

    const std::vector<AttachmentConfig>& GetColorAttachments() const { return attachments_; }

    const RHI::TextureView* GetDSAttachment() const { return DepthStencilAttachment_.RenderTarget; }

    RHI::RenderPassEncoder* GetRenderPassEncoder() const { return rhiPassEncoder_; }

    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) = 0;

protected:
    RHI::RenderPassEncoder* RenderPassEncoder_ = nullptr;
    RHI::CommandEncoder* CommandEncoder_ = nullptr;
    std::vector<AttachmentConfig> attachments_;
    AttachmentConfig DepthStencilAttachment_;
    RHI::RenderPassEncoder* rhiPassEncoder_ = nullptr;
};

class IgniterPass : public Pass
{
public:
    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) override;
};

class ShadowMapGenPass : public Pass
{
public:
    ShadowMapGenPass();

    virtual ~ShadowMapGenPass()
    {

    }

    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) override;

protected:
    TExtent2D     shadowMapSize_ = { 2048, 2048 };
    //RHI::TextureView* shadowMapTexture_ = nullptr;
    RHI::TextureView* dsTexture_ = nullptr;
};

struct GBuffers
{
    RHI::TextureView* GDiffuse = nullptr;
    RHI::TextureView* GNormal = nullptr;
    RHI::TextureView* GPosition = nullptr;
    RHI::TextureView* GMaterial = nullptr;
};

class GBufferPass : public Pass
{
public:
    GBufferPass();
    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) override;

    const GBuffers& GetGBuffers() { return GBuffers_; }

private:
    GBuffers GBuffers_;
    RHI::TextureView* dsTexture_ = nullptr;
};

class OpaquePass : public Pass
{
public:
    OpaquePass();
    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) override;

protected:
    RHI::TextureView* rtColor_ = nullptr;
    RHI::TextureView* rtDepthStencil_ = nullptr;
};

class SkyBoxPass : public Pass
{
public:
    SkyBoxPass();
    void Setup(const Pass* mainPass, const Pass* depthPass);
    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) override;

protected:
    RHI::TextureView* rtColor_ = nullptr;
    RHI::TextureView* rtDepthStencil_ = nullptr;
};

class FullScreenPass : public Pass
{
public:
    FullScreenPass(const std::string& shaderName, ETechniqueType technique);

    void Execute(RHI::CommandEncoder* cmdEncoder);

    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) override {}

protected:
    MeshComponent* meshComponent_ = nullptr;
};

class DeferredPass : public FullScreenPass
{
public:
    DeferredPass();
    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) override;

    void Setup(const Pass* smPass)
    {
        shadowMapPass_ = smPass;
    }

protected:
    GBufferPass GBufferPass_;
    RHI::TextureView* rtColor_ = nullptr;
    const Pass* shadowMapPass_ = nullptr;
};

class ScreenResolvePass : public FullScreenPass
{
public:
    ScreenResolvePass()
        : FullScreenPass("Materials/ScreenResolve.json", ETechniqueType::TShading)
    {}

    void Setup(const Pass* inputPass)
    {
        inputPass_ = inputPass;
    }

    void Execute(RHI::CommandEncoder* cmdEncoder);

protected:
    const Pass* inputPass_ = nullptr;
};

NS_RX_END


#endif //ALITA_PASS_H
