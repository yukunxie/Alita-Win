//
// Created by realxie on 2022-01-06.
//

#pragma once

#include "Base/ObjectBase.h"
#include "GFX/GFX.h"
#include "Types/Types.h"
#include "World/Camera.h"
#include "Technique.h"
#include "RenderTarget.h"
#include "Texture.h"

NS_RX_BEGIN

class FrameGraphNode : public ObjectBase
{
public:
	FrameGraphNode() = default;

	~FrameGraphNode() = default;

public:
	void AddDependentNode(FrameGraphNode* node);
};

NS_RX_END


