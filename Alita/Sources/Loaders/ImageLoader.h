#pragma once

#include "Base/ObjectBase.h"
#include "World/Model.h"
#include "RHI.h"


NS_RX_BEGIN

namespace ImageLoader
{
	RHI::Texture* LoadTextureFromUri(const std::string& filename);
}

NS_RX_END
