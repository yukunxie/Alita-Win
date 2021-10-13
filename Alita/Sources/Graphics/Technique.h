//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_TECHNIQUE_H
#define ALITA_TECHNIQUE_H

#include "../Base/ObjectBase.h"

NS_RX_BEGIN

enum class ETechniqueType
{
    TShading        = 0 , 
    TGBufferGen         ,
    TShadowMapGen       ,
    TOutline            ,
    TSkyBox             ,
    TVolumeCloud        ,

    TMaxCount           ,
};

enum ERenderSet : std::uint64_t
{
    ERenderSet_Opaque           = 1 << 0,
    ERenderSet_Transparent      = 1 << 1,
    ERenderSet_SkyBox           = 1 << 2,
    ERenderSet_PostProcess      = 1 << 3,
};

enum ETechniqueMask : std::uint64_t
{
    TShading    = 1 << (uint32)ETechniqueType::TShading,
    TGBufferGen = 1 << (uint32)ETechniqueType::TGBufferGen,
};

using TechniqueFlags = std::uint64_t;

constexpr uint32 kMaxAttachmentCount = 6;
//
//class Technique
//{
//public:
//};

NS_RX_END

#endif //ALITA_TECHNIQUE_H
