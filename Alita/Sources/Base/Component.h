//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_COMPONENT_H
#define ALITA_COMPONENT_H

#include "Macros.h"
#include "ObjectBase.h"
#include "Types/Types.h"

NS_RX_BEGIN

class Entity;

class Component : public ObjectBase
{
public:
    void SetEntity(Entity* entity);

protected:
    Entity* owner_ = nullptr;
};

class Transform : Component
{
public:
    TVector3& Position()
    {
        return position_;
    }

    void SetPosition(const TVector3& position)
    {
        position_ = position;
    }

    TVector3& Scale()
    {
        return scale_;
    }

    void SetScale(const TVector3& scale)
    {
        scale_ = scale;
    }

    TVector3& Rotation()
    {
        return rotation_;
    }

    void SetRotation(const TVector3& rotation)
    {
        rotation_ = rotation;
    }
    
protected:
    TVector3 position_ = { 0, 0, 0 };
    TVector3 scale_ = { 1, 1, 1 };
    TVector3 rotation_ = { 0, 0, 0 };
};



NS_RX_END

#endif //ALITA_COMPONENT_H
