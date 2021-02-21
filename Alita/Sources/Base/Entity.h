//
// Created by realxie on 2019-10-29.
//

#ifndef ALITA_ENTITY_H
#define ALITA_ENTITY_H

#include "ObjectBase.h"
#include "Component.h"

#include <list>

NS_RX_BEGIN

class Entity : public ObjectBase
{
public:
    void AddComponment(Component* componment);
    
    /*
     * Get component with RTTI
     * */
    template <typename Tp_>
    Tp_* GetComponent()
    {
        if (dynamic_cast<Tp_*>(&transform_))
        {
            return dynamic_cast<Tp_*>(&transform_);
        }

        for (auto comp : components_)
        {
            if (dynamic_cast<Tp_*>(comp))
            {
                return dynamic_cast<Tp_*>(comp);
            }
        }
        return nullptr;
    }

    virtual void Tick(float dt) {}
    
protected:
    std::list<Component*> components_;

    // All entity has a transform attribute
    Transform transform_;
};

NS_RX_END


#endif //ALITA_ENTITY_H
