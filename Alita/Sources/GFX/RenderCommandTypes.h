//
// Created by realxie on 2020-02-25.
//

#ifndef PROJ_ANDROID_RENDERCOMMANDTYPES_H
#define PROJ_ANDROID_RENDERCOMMANDTYPES_H

#include "Macros.h"

NS_GFX_BEGIN

enum class RenderCommand
{
    INVALID = -1,
    BeginCommandBuffer,
    BeginComputePass,
    BeginRenderPass,
    ClearAttachment,
    CopyBufferToBuffer,
    CopyBufferToTexture,
    CopyTextureToBuffer,
    CopyTextureToTexture,
    Dispatch,
    DispatchIndirect,
    Draw,
    DrawIndexed,
    DrawIndirect,
    DrawIndexedIndirect,
    EndComputePass,
    EndPass,
    SetComputePipeline,
    SetGraphicPipeline,
    SetPushConstants,
    SetStencilReference,
    SetScissorRect,
    SetViewport,
    SetBlendColor,
    SetDepthBias,
    SetBindGroup,
    SetIndexBuffer,
    SetVertexBuffer,
    PushDebugGroup,
    PopDebugGroup,
    InsertDebugMarker,
    BeginOcclusionQuery,
    EndOcclusionQuery,
    ResolveQuerySet,
    PipelineBarrier,
};

NS_GFX_END

#endif //PROJ_ANDROID_RENDERCOMMANDTYPES_H
