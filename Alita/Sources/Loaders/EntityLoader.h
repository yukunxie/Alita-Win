#pragma once

#include "Base/ObjectBase.h"
#include "World/Model.h"

NS_RX_BEGIN

namespace EntityLoader
{
	Model* LoadModelFromGLTF(const std::string& gltfFilename);
}

NS_RX_END


