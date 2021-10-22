//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_PASS_H
#define ALITA_PASS_H

#include "Base/ObjectBase.h"
#include "GFX/GFX.h"
#include "Types/Types.h"
#include "World/Camera.h"
#include "Technique.h"
#include "RenderTarget.h"
#include "Texture.h"

NS_RX_BEGIN

enum class PassType
{
    OPAQUE,
    TRANSPARENT,
    UI,
};

class MeshComponent;

class RenderObject;

class Material;

struct AttachmentConfig
{
    std::shared_ptr<RenderTarget> RenderTarget= nullptr;
    uint8               Slot            = 0xFF;
    bool                Clear           = true;
    
};

class Pass : public ObjectBase
{
public:
    void SetupDepthStencilAttachemnt(std::shared_ptr<RenderTarget> attachment, bool clear = true)
    {
        DepthStencilAttachment_.RenderTarget = attachment;
        DepthStencilAttachment_.Clear = clear;
    }

    void SetupOutputAttachment(std::uint32_t index, std::shared_ptr<RenderTarget> attachment, bool clear = true)
    {
        attachments_.push_back(AttachmentConfig
            {
                .RenderTarget = attachment,
                .Slot = (uint8)index,
                .Clear = clear
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

    const std::shared_ptr<RenderTarget> GetDSAttachment() const { return DepthStencilAttachment_.RenderTarget; }

    gfx::RenderPassEncoder* GetRenderPassEncoder() const { return rhiPassEncoder_; }

    virtual void Execute(const std::vector<RenderObject*>& renderObjects) = 0;

protected:
    std::string PassName_ = "GiveAPassName:(";
    gfx::RenderPassEncoder* RenderPassEncoder_ = nullptr;
    gfx::CommandEncoder* CommandEncoder_ = nullptr;
    std::vector<AttachmentConfig> attachments_;
    AttachmentConfig DepthStencilAttachment_;
    gfx::RenderPassEncoder* rhiPassEncoder_ = nullptr;
};

class IgniterPass : public Pass
{
public:
    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override;
};

class ShadowMapGenPass : public Pass
{
public:
    ShadowMapGenPass();

    virtual ~ShadowMapGenPass()
    {

    }

    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override;

protected:
    TExtent2D     shadowMapSize_ = { 2048, 2048 };
    std::shared_ptr<RenderTarget> dsTexture_ = nullptr;
};

struct GBuffers
{
    std::shared_ptr<RenderTarget> GDiffuse = nullptr;
    std::shared_ptr<RenderTarget> GEmissive = nullptr;
    std::shared_ptr<RenderTarget> GNormal = nullptr;
    std::shared_ptr<RenderTarget> GPosition = nullptr;
    std::shared_ptr<RenderTarget> GMaterial = nullptr;
};

class GBufferPass : public Pass
{
public:
    GBufferPass();
    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override;

    const GBuffers& GetGBuffers() { return GBuffers_; }

private:
    GBuffers GBuffers_;
    std::shared_ptr<RenderTarget> dsTexture_ = nullptr;
};

class OpaquePass : public Pass
{
public:
    OpaquePass();
    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override;

protected:
    std::shared_ptr<RenderTarget> rtColor_ = nullptr;
    std::shared_ptr<RenderTarget> rtDepthStencil_ = nullptr;
};

class SkyBoxPass : public Pass
{
public:
    SkyBoxPass();
    void Setup(const Pass* mainPass, const Pass* depthPass);
    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override;

protected:
    std::shared_ptr<RenderTarget> rtColor_ = nullptr;
    std::shared_ptr<RenderTarget> rtDepthStencil_ = nullptr;
};

class FullScreenPass : public Pass
{
public:
    FullScreenPass(const std::string& shaderName, ETechniqueType technique);

    void Execute();

    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override {}

protected:
    void SetTexture(const std::string& name, const gfx::Texture* texture);

    void SetFloat(const std::string& name, std::uint32_t offset, std::uint32_t count, const float* data);

    Material* GetMaterial();

protected:
    MeshComponent* meshComponent_ = nullptr;
};

class DeferredPass : public FullScreenPass
{
public:
    DeferredPass();
    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override;

    void Setup(const Pass* smPass)
    {
        shadowMapPass_ = smPass;
    }

protected:
    GBufferPass GBufferPass_;
    std::shared_ptr<RenderTarget> rtColor_ = nullptr;
    const Pass* shadowMapPass_ = nullptr;
};

class ScreenResolvePass : public FullScreenPass
{
public:
    ScreenResolvePass()
        : FullScreenPass("Materials/ScreenResolve.json", ETechniqueType::TShading)
    {
        RTSwapChain_ = std::make_shared<RenderTargetSwapChain>();
    }

    void Setup(const Pass* inputPass)
    {
        inputPass_ = inputPass;
    }

    void Execute();

protected:
    const Pass* inputPass_ = nullptr;

    std::shared_ptr<RenderTargetSwapChain> RTSwapChain_ = nullptr;
};

class DownSamplePass : public FullScreenPass
{
public:
    enum DownSampleFilterType
    {
        Average = 1,
        Min     = 2,
        Max     = 3
    };
public:
    DownSamplePass();

    void Execute();

    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override {};

    void Setup(const Pass* inputPass)
    {
        InputPass_ = inputPass;
    }

    void SetDownSampleFilterType(DownSampleFilterType type)
    {
        FilterType_ = type;
    }

protected:
    DownSampleFilterType    FilterType_     = DownSampleFilterType::Average;
    std::shared_ptr<RenderTarget> RTColor_  = nullptr;
    const Pass* InputPass_                  = nullptr;
};

class BloomBrightPass : public FullScreenPass
{
public:
    BloomBrightPass();

    void Execute();

    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override {};

    void Setup(const Pass* inputPass)
    {
        InputPass_ = inputPass;
    }

protected:
    std::shared_ptr<RenderTarget> RTColor_ = nullptr;
    const Pass* InputPass_ = nullptr;
};

class GaussianBlur : public FullScreenPass
{
public:
    GaussianBlur();

    void Execute();

    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override {};

    void Setup(const Pass* inputPass);

protected:
    GaussianBlur(bool isVertical);

protected:
    TVector4  Param;
    std::shared_ptr<GaussianBlur> VerticalGuassianBlurPass_;
    std::shared_ptr<RenderTarget> RTColor_ = nullptr;
    const Pass* InputPass_ = nullptr;
};

class OutlineMarkPass : public Pass
{
public:
    OutlineMarkPass();

    virtual ~OutlineMarkPass()
    {
    }

    void ResizeTarget(uint32 width, uint32 height);

    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override;

protected:
    std::shared_ptr<RenderTarget> rtColor_ = nullptr;
    std::shared_ptr<RenderTarget> rtDepthStencil_ = nullptr;
};

class OutlinePass : public FullScreenPass
{
public:
    OutlinePass();

    virtual ~OutlinePass()
    {
    }

    void Setup(const Pass* inputPass);

    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override;

protected:
    std::shared_ptr<RenderTarget> rtColor_ = nullptr;

    const Pass* InputPass_ = nullptr;
    OutlineMarkPass OutlineMarkPass_;
    GaussianBlur    BlurPass_;

};

class CloudPass : public Pass
{
public:
    CloudPass();

    virtual void Execute(const std::vector<RenderObject*>& renderObjects) override
    {
        Execute();
    }

    void Setup(const Pass* inputPass, const Pass* depthPass)
    {
        InputPass_ = inputPass;
        DepthPass_ = depthPass;
    }

protected:
    void Execute();

protected:
    const Pass* InputPass_ = nullptr;
    const Pass* DepthPass_ = nullptr;

    TSharedPtr<MeshComponent> SphereMeshComponent_;
    std::shared_ptr<Texture> NoiseMap3D_ = nullptr;
};

NS_RX_END


#endif //ALITA_PASS_H
