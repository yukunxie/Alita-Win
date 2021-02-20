//
// Created by realxie on 2019-10-11.
//

#ifndef ALITA_COMMANDENCODER_H
#define ALITA_COMMANDENCODER_H

#include "Macros.h"
#include "RHIObjectBase.h"
#include "CommandBuffer.h"
#include "RenderPassEncoder.h"
#include "Descriptors.h"
#include "Buffer.h"

NS_RHI_BEGIN

class CommandEncoder : public RHIObjectBase
{
public:
    virtual ~CommandEncoder()
    {
    }

public:
    virtual RenderPassEncoder* BeginRenderPass(const RenderPassDescriptor& descriptor) = 0;

    virtual CommandBuffer* Finish(const CommandBufferDescriptor& descriptor = {}) = 0;

    virtual void CopyBufferToBuffer(
            const Buffer* source,
            BufferSize sourceOffset,
            Buffer* destination,
            BufferSize destinationOffset,
            BufferSize size) = 0;

    virtual void CopyBufferToTexture(
            const BufferCopyView& source,
            TextureCopyView& destination,
            const Extent3D& copySize) = 0;

//    void copyTextureToBuffer(
//            GPUTextureCopyView source,
//            GPUBufferCopyView destination,
//            GPUExtent3D copySize);
//
//    void copyTextureToTexture(
//            GPUTextureCopyView source,
//            GPUTextureCopyView destination,
//            GPUExtent3D copySize);
//
//    void copyImageBitmapToTexture(
//            GPUImageBitmapCopyView source,
//            GPUTextureCopyView destination,
//            GPUExtent3D copySize);
//
//    void pushDebugGroup(DOMString groupLabel);
//    void popDebugGroup();
//    void insertDebugMarker(DOMString markerLabel);
};

NS_RHI_END


#endif //ALITA_COMMANDENCODER_H
