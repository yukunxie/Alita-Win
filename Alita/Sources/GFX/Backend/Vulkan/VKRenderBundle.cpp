//
// Created by realxie on 2020-03-24.
//

#include "VKRenderBundle.h"

NS_GFX_BEGIN

VKRenderBundle::VKRenderBundle(VKDevice* device)
: RenderBundle(device)
{
}

bool VKRenderBundle::Init()
{
    return true;
}

NS_GFX_END
