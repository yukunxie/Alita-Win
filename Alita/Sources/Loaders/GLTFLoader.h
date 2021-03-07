#pragma once

#include "Base/ObjectBase.h"
#include "World/Model.h"
#include "RHI.h"


NS_RX_BEGIN

namespace GLTFLoader
{
	std::vector<Entity*> LoadModelFromGLTF(const std::string& filename);
}

NS_RX_END
