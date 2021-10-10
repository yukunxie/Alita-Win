//
// Created by realxie on 2019-10-25.
//

#ifndef ALITA_WORLD_H
#define ALITA_WORLD_H

#include "Base/Macros.h"
#include "SceneComponent.h"
#include "Camera.h"
#include "Physics/Physics.h"

NS_RX_BEGIN

class Camera;

class World : public ObjectBase
{
public:
    World();
    
    void Tick(float dt);

    void Render();

    Camera* GetCamera() const
    {
        return defaultCamera_;
    }

    TVector3 GetCameraPosition()
    {
        return defaultCamera_->GetComponent<Transform>()->Position();
    }

    Camera* GetShadowMapCamera()
    {
        return shadowMapCamera_;
    }

    void AddEntity(Entity* entity);

private:
    void SetupDefaultCamera();

protected:

    std::vector<Entity*> entities_;

    //std::vector<Level*> levels_;

    Camera* defaultCamera_ = nullptr;
    Camera* shadowMapCamera_ = nullptr;

public:
    MeshComponent* CurrentSelectedMeshComponent = nullptr;

};

NS_RX_END

#endif //ALITA_WORLD_H
