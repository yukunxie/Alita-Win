//
// Created by realxie on 2019-09-24.
//

#ifndef ALITA_TYPES_H
#define ALITA_TYPES_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


using TVector4 = glm::vec4;
using TVector3 = glm::vec3;
using TVector2 = glm::vec2;
using TMat4x4 = glm::mat4;

using TColor4 = glm::vec4;
using TColor3 = glm::vec3;

struct TVertex {
    TVector3 pos;
    TVector4 color;
    TVector2 texCoord;
};

struct TExtent2D
{
    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

struct TExtent3D
{
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t depth = 0;
};

#endif //ALITA_TYPES_H
