#pragma once


#include "Base/Macros.h"
#include <vector>

NS_RX_BEGIN

enum IndexType
{
	UINT32,
	UINT16
};

enum class MaterialParameterType
{
	FLOAT,
	FLOAT2,
	FLOAT3,
	FLOAT4,

	INT,
	INT2,
	INT3,
	INT4,

	BOOL,
	BOOL1,
	BOOL2,
	BOOL3,

	MAT4,
	MAT3,
	MAT2,
	MAT4x3,
	MAT4x2,
	MAT3x4,
	MAT2x4,
	MAT3x2,
	MAT2x3,

	BUFFER,
	SAMPLER2D,
	TEXTURE2D,
};

enum class InputAttributeFormat
{
	FLOAT,
	FLOAT2,
	FLOAT3,
	FLOAT4,
};


enum VertexBufferAttriKind
{
	INVALID = 0,
	POSITION = 0x1, // xyz
	DIFFUSE = 0x2, // rgba 4byte
	TEXCOORD = 0x4, // uv 2floats
	NORMAL = 0x8,  // xyz
	TANGENT = 0x10, // xyz
	BINORMAL = 0x20, // xyz
	BITANGENT = 0x40, // xyz
	TEXCOORD2 = 0x80, // uv2 2floats
};

constexpr std::uint32_t VertexBufferKindCount = 7;

enum class MaterailBindingObjectType
{
	BUFFER,
	SAMPLER2D,
	TEXTURE2D,
};

enum InputAttributeLocation
{
	IA_LOCATION_POSITION = 0,
	IA_LOCATION_NORMAL = 1,
	IA_LOCATION_TEXCOORD = 2,
	IA_LOCATION_DIFFUSE = 3,
	IA_LOCATION_TANGENT = 4,
	IA_LOCATION_BINORMAL = 5,
	IA_LOCATION_BITANGENT = 6,
	IA_LOCATION_TEXCOORD2 = 7,
};

NS_RX_END
