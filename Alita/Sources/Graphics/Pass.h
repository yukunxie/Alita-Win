//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_PASS_H
#define ALITA_PASS_H

#include "Base/ObjectBase.h"
#include "RHI.h"

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

    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) = 0;

protected:
    std::vector<std::pair<std::uint32_t, RHI::TextureView*>> attachments_;
    RHI::TextureView* dsAttachment_ = nullptr;
};

class IgniterPass : public Pass
{
public:
    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) override;
};

class OpaquePass : public Pass
{
public:
    virtual void Execute(RHI::CommandEncoder* cmdEncoder, const std::vector<RenderObject*>& renderObjects) override;
};

NS_RX_END


#endif //ALITA_PASS_H
