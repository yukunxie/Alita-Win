#include "SkyBox.h"
#include "stb/stb_image.h"
#include "Base/FileSystem.h"
#include "World/MeshComponent.h"
#include "World/World.h"
#include "Graphics/RenderScene.h"

#include <math.h>

NS_RX_BEGIN

SkyBox::SkyBox()
{
    auto modelComp = MeshComponentBuilder::CreateBox("Materials/SkyBox.json");
    modelComp->SetRenderSet(ERenderSet_SkyBox);
    this->AddComponment(modelComp);
    this->SetScale({ 500, 500, 500 });
}

SkyBox::~SkyBox()
{
}

void SkyBox::Tick(float dt)
{
    SetPosition(Engine::GetWorld()->GetCameraPosition());
    Model::Tick(dt);
}


NS_RX_END