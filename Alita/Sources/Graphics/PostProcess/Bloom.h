//
// Created by realxie on 2019-10-29.
//

#pragma once

#include "Base/ObjectBase.h"
#include "RHI/RHI.h"
#include "Types/Types.h"
#include "World/Camera.h"
#include "../Pass.h"

NS_RX_BEGIN

class Material;

class BloomPass : public FullScreenPass
{
public:
    BloomPass()
        : FullScreenPass("Materials/Bloom.json", ETechniqueType::TShading)
    {
        PassName_ = "BloomPass";
        RTColor_ = std::make_shared<RenderTarget>();
    }

    void Setup(const Pass* inputPass);

    void Execute();

protected:
    const Pass*     InputPass_ = nullptr;
    BloomBrightPass BloomBrightPass_;
    DownSamplePass  DownSamplePass_;
    GaussianBlur    GaussianBlurPass_;
    std::shared_ptr<RenderTarget> RTColor_ = nullptr;
};

NS_RX_END

