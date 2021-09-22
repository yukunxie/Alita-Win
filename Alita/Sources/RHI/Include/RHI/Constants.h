//
// Created by realxie on 2019-10-24.
//

#ifndef RHI_CONSTANTS_H
#define RHI_CONSTANTS_H

#include "Macros.h"
#include <stdint.h>
#include <limits>

NS_RHI_BEGIN

static constexpr std::uint32_t kMaxPushConstants = 32u;
static constexpr std::uint32_t kMaxBindGroups = 8u;
static constexpr std::uint32_t kMaxBindingsPerGroup = 16u;
static constexpr std::uint32_t kMaxVertexAttributes = 16u;
static constexpr std::uint32_t kMaxVertexInputs = 16u;
static constexpr std::uint32_t kNumStages = 3;
static constexpr std::uint32_t kMaxColorAttachments = 8u;
static constexpr std::uint32_t kMaxAttachments = kMaxColorAttachments + 1u; // colors + depth-stencil
static constexpr std::uint32_t kTextureRowPitchAlignment = 256u;
static constexpr std::uint32_t kMaxDynamicArraySize = 16u;
static constexpr std::uint32_t kMaxSubmitCommandBuffers = 8u;

static constexpr std::uint64_t kMaxUint64 = std::numeric_limits<uint64_t>::max();
static constexpr std::uint64_t kMaxUint32 = std::numeric_limits<uint32_t>::max();

NS_RHI_END

#endif //RHI_CONSTANTS_H
