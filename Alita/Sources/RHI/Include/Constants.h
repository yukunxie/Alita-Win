//
// Created by realxie on 2019-10-24.
//

#ifndef ALITA_CONSTANTS_H
#define ALITA_CONSTANTS_H

#include "Macros.h"
#include <stdint.h>

NS_RHI_BEGIN

static constexpr std::uint32_t kMaxPushConstants = 32u;
static constexpr std::uint32_t kMaxBindGroups = 4u;
static constexpr std::uint32_t kMaxBindingsPerGroup = 16u;
static constexpr std::uint32_t kMaxVertexAttributes = 16u;
static constexpr std::uint32_t kMaxVertexInputs = 16u;
static constexpr std::uint32_t kNumStages = 3;
static constexpr std::uint32_t kMaxColorAttachments = 4u;
static constexpr std::uint32_t kTextureRowPitchAlignment = 256u;

NS_RHI_END

#endif //ALITA_CONSTANTS_H
