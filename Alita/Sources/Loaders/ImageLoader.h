#pragma once

#include "Base/ObjectBase.h"
#include "World/Model.h"
#include "GFX/GFX.h"


NS_RX_BEGIN

namespace ImageLoader
{
	gfx::TexturePtr LoadTextureFromUri(const std::string& filename);
	gfx::TexturePtr LoadCubeTexture(const std::string& cubeTextureName);
	gfx::TexturePtr LoadTextureFromData(const std::uint8_t* data, std::uint32_t byteLength, const std::string& debugName = "");
	gfx::TexturePtr LoadTextureFromData(uint32 width, uint32 height, uint32 component, const uint8* data, uint32 byteLength, const std::string& debugName = "");
	gfx::TexturePtr Create3DFloatTexture(uint32 width, uint32 height, uint32 depth);
}

NS_RX_END
