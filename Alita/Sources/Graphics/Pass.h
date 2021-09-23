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

class Pass : public ObjectBase
{
public:
    void SetupDepthStencilAttachemnt(RHI::TextureView* attachment)
    {
        dsAttachment_ = attachment;
    }

    void SetupOutputAttachment(std::uint32_t index, RHI::TextureView* attachment)
    {
        attachments_.push_back({ index, attachment });
    }

    void Reset()
    {
        attachments_.clear();
        dsAttachment_ = nullptr;
    }

    const std::vector<std::pair<std::uint32_t, RHI::TextureView*>>& GetColorAttachments() const { return attachments_; }

    const RHI::TextureView* GetDSAttachment() const { return dsAttachment_; }

    RHI::RenderPassEncoder* GetRenderPassEncoder() const { return rhiPassEncoder_; }

    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) = 0;

protected:
    std::vector<std::pair<std::uint32_t, RHI::TextureView*>> attachments_;
    RHI::TextureView* dsAttachment_ = nullptr;
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
    RHI::TextureView* shadowMapTexture_ = nullptr;
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

protected:
    GBufferPass GBufferPass_;
    RHI::TextureView* rtColor_ = nullptr;
};

class ScreenResolvePass : public FullScreenPass
{
public:
    ScreenResolvePass()
        : FullScreenPass("Materials/PostProcess.json", ETechniqueType::TShading)
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
