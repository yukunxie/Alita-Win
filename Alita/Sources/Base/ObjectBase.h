//
// Created by realxie on 2019-10-25.
//

#ifndef ALITA_OBJECTBASE_H
#define ALITA_OBJECTBASE_H

#include "Macros.h"
#include "Types/Enums.h"

#include <stdint.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <limits>
#include <unordered_map>
#include <algorithm>
#include <set>

NS_RX_BEGIN

#define SET_CLASS_NON_COPYABLE(ClassName)               \
public:                                                 \
    ClassName(ClassName&&) = delete;                    \
    ClassName& operator =(ClassName&&) = delete;        \
    ClassName(const ClassName&) = delete;               \
    ClassName& operator=(const ClassName&) = delete;

    
class ObjectBase
{
SET_CLASS_NON_COPYABLE(ObjectBase)

protected:
    virtual ~ObjectBase() = default;

public:
    ObjectBase() = default;

    void Retain()
    {
        ++__referenceCount_;
    }

    void Release()
    {
        --__referenceCount_;
        if (__referenceCount_ <= 0)
        {
            OnDeleteEvent();
            delete this;
        }
    }

    std::int32_t GetReferenceCount() const
    {
        return __referenceCount_;
    }

    virtual void OnDeleteEvent()
    {
        // to override this function to process some logic before delete
    }

private:
    std::int32_t __referenceCount_ = 0;
};

NS_RX_END


#endif //ALITA_OBJECTBASE_H
