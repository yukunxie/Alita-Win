//
// Created by realxie on 2019/3/23.
//

#ifndef ALITA_ALITA_H
#define ALITA_ALITA_H

#include <vector>
//#include <android_native_app_glue.h>
#include "types/Types.h"
#include "aux/Aux.h"

#ifdef ANDROID
#include <android/native_window.h>
#include <android/log.h>
#endif

#include "RHI/Include/RHI.h"
#include "../RHI/Include/CanvasContext.h"

class RealRenderer
{
public:
    virtual ~RealRenderer();

    static RealRenderer *getInstance();

    inline bool isReady()
    { return rhiDevice_ != nullptr; }

    bool initVulkanContext(ANativeWindow* app);

    void drawFrame();

    void testRotate();

protected:
    RealRenderer();

private:
    static RealRenderer *instance_;

    RHI::CanvasContext*   rhiCanvasContext_     = nullptr;
    RHI::Device*          rhiDevice_            = nullptr;
    RHI::Buffer*          rhiVertexBuffer_      = nullptr;
    RHI::Buffer*          rhiIndexBuffer_       = nullptr;
    RHI::Buffer*          rhiUniformBuffer_     = nullptr;
    RHI::BindGroup*       rhiBindGroup_         = nullptr;
    RHI::BindingResource* rhiBindingBuffer_     = nullptr;
    RHI::BindingResource* rhiBindingCombined_   = nullptr;
    RHI::PipelineLayout*  rhiPipelineLayout_    = nullptr;
    RHI::BindGroupLayout* rhiBindGroupLayout_   = nullptr;
    RHI::Texture*         rhiTexture_           = nullptr;
    RHI::TextureView*     rhiTextureView_       = nullptr;
    RHI::Texture*         rhiDSTexture_         = nullptr;
    RHI::TextureView*     rhiDSTextureView_     = nullptr;
    RHI::Sampler*         rhiSampler_           = nullptr;
    RHI::RenderPipeline*  rhiGraphicPipeline_   = nullptr;
    RHI::Shader*          rhiVertShader_        = nullptr;
    RHI::Shader*          rhiFragShader_        = nullptr;
    RHI::CommandEncoder*  rhiCommandEncoder_    = nullptr;
    RHI::SwapChain*       rhiSwapChain_         = nullptr;
};
#endif //ALITA_ALITA_H
