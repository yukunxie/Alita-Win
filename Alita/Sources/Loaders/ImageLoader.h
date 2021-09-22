#pragma once

#include "Base/ObjectBase.h"
#include "World/Model.h"
#include "RHI/RHI.h"


NS_RX_BEGIN

namespace ImageLoader
{
	RHI::Texture* LoadTextureFromUri(const std::string& filename);
	RHI::Texture* LoadTextureFromData(const std::uint8_t* data, std::uint32_t byteLength);
	RHI::Texture* LoadTextureFromData(uint32 width, uint32 height, uint32 component, const uint8* data, uint32 byteLength);
}

NS_RX_END
