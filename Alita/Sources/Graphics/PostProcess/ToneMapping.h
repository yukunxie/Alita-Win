//
// Created by realxie on 2019-10-29.
//

#pragma once

#include "Base/ObjectBase.h"
#include "GFX/GFX.h"
#include "Types/Types.h"
#include "World/Camera.h"
#include "../Pass.h"

NS_RX_BEGIN

class Material;

class ToneMapping : public FullScreenPass
{
public:
    ToneMapping()
        : FullScreenPass("Materials/ToneMapping.json", ETechniqueType::TShading)
    {
        PassName_ = "ToneMappingPass";
        RTColor_ = std::make_shared<RenderTarget>("ToneMapping");
    }

    void Setup(const Pass* inputPass);

    void Execute();

protected:
    const Pass*     InputPass_ = nullptr;
    std::shared_ptr<RenderTarget> RTColor_ = nullptr;
};

NS_RX_END

