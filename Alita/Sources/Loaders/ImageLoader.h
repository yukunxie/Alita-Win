#pragma once

#include "Base/ObjectBase.h"
#include "World/Model.h"
#include "GFX/GFX.h"


NS_RX_BEGIN

namespace ImageLoader
{
	gfx::Texture* LoadTextureFromUri(const std::string& filename);
	gfx::Texture* LoadCubeTexture(const std::string& cubeTextureName);
	gfx::Texture* LoadTextureFromData(const std::uint8_t* data, std::uint32_t byteLength, const std::string& debugName = "");
	gfx::Texture* LoadTextureFromData(uint32 width, uint32 height, uint32 component, const uint8* data, uint32 byteLength, const std::string& debugName = "");
	gfx::Texture* Create3DFloatTexture(uint32 width, uint32 height, uint32 depth);
}

NS_RX_END
