#pragma once

#include "Base/ObjectBase.h"
#include "World/Model.h"
#include "RHI/RHI.h"


NS_RX_BEGIN

namespace ImageLoader
{
	RHI::Texture* LoadTextureFromUri(const std::string& filename);
	RHI::Texture* LoadCubeTexture(const std::string& cubeTextureName);
	RHI::Texture* LoadTextureFromData(const std::uint8_t* data, std::uint32_t byteLength, const std::string& debugName = "");
	RHI::Texture* LoadTextureFromData(uint32 width, uint32 height, uint32 component, const uint8* data, uint32 byteLength, const std::string& debugName = "");
}

NS_RX_END
