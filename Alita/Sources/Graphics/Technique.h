//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_TECHNIQUE_H
#define ALITA_TECHNIQUE_H

#include "../Base/ObjectBase.h"
#include "Pass.h"

NS_RX_BEGIN

enum class ETechniqueType
{
    TShading    = 0, 
    TGBufferGen,

    TMaxCount,
};

enum ETechniqueMask : std::uint64_t
{
    TShading    = 1 << (uint32)ETechniqueType::TShading,
    TGBufferGen = 1 << (uint32)ETechniqueType::TGBufferGen,
};

using TechniqueFlags = std::uint64_t;
//
//class Technique
//{
//public:
//};

NS_RX_END

#endif //ALITA_TECHNIQUE_H
