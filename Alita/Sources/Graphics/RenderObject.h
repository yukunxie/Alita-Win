#pragma once

#include "RHI.h"

#include "Base/ObjectBase.h"


NS_RX_BEGIN

class RenderObject
{
protected:
	std::vector<RHI::Buffer*> vertexBuffers_;
	RHI::Buffer* indexBuffer_ = nullptr;
};

NS_RX_END
