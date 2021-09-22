//
// Created by realxie on 2019-10-04.
//

#ifndef ALITA_RENDERPIPEINE_H
#define ALITA_RENDERPIPEINE_H

#include "Macros.h"
#include "Flags.h"
#include "BindGroupLayout.h"
#include "PipelineLayout.h"
#include "RHIObjectBase.h"


NS_RHI_BEGIN

typedef struct GraphicPipelineCreateInfo
{
    std::vector<PipelineShaderStageCreateInfo> shaderStageInfos;
    PipelineVertexInputStateCreateInfo vertexInputInfo;
    PipelineViewportStateCreateInfo viewportState;
    const PipelineLayout* pPipelineLayout;
    
} GraphicPipelineCreateInfo;


class RenderPipeline : public RHIObjectBase
{
public:
    virtual ~RenderPipeline()
    {
    }
};

NS_RHI_END

#endif //ALITA_RENDERPIPEINE_H
