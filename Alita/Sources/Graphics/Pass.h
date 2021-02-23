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

class Pass : public ObjectBase
{
public:
    void SetupOutputAttachment(std::uint32_t index, RHI::TextureView* attachment)
    {
        attachments_.push_back({ index, attachment });
    }
    virtual void Execute() = 0;

protected:
    std::vector<std::pair<std::uint32_t, RHI::TextureView*>> attachments_;
};

class OpaquePass : public Pass
{
    virtual void Execute() override
    {

    }
};

NS_RX_END


#endif //ALITA_PASS_H
