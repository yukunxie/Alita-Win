#pragma once

#include "Base/ObjectBase.h"
#include "World/Model.h"
#include "RHI.h"


NS_RX_BEGIN

namespace ImageLoader
{
	RHI::Texture* LoadTextureFromUri(const std::string& filename);
	RHI::Texture* LoadTextureFromData(const std::uint8_t* data, std::uint32_t byteLength);
	RHI::Texture* LoadTextureFromData(int width, int height, int component, const std::uint8_t* data, std::uint32_t byteLength);
}

NS_RX_END
