//
// Created by realxie on 2020-03-02.
//

#ifndef PROJ_ANDROID_TURBOVECTOR_H
#define PROJ_ANDROID_TURBOVECTOR_H

#include "Macros.h"
#include <array>
#include <vector>

NS_RHI_BEGIN

template<typename Tp_, size_t DefaultSize = 16>
class TurboVector final
{
public:
    TurboVector() = default;
    
    TurboVector(size_t size, const Tp_ &value)
    {
        if (stackDatas_.size() < size)
        {
            heapDatas_ = new std::vector<Tp_>(size, value);
        }
        else
        {
            stackDatas_.fill(value);
        }
        elementCount_ = size;
    }
    
    TurboVector(size_t size)
    {
        if (stackDatas_.size() < size)
        {
            heapDatas_ = new std::vector<Tp_>(size);
        }
        elementCount_ = size;
    }
    
    ~TurboVector()
    {
        if (heapDatas_)
        {
            delete heapDatas_;
        }
    }
    
    TurboVector<Tp_, DefaultSize> &operator=(const TurboVector<Tp_, DefaultSize> &other)
    {
        this->clear();
        resize(other.size());
        for (size_t i = 0; i < other.size(); ++i)
        {
            (*this)[i] = other[i];
        }
        return *this;
    }
    
    Tp_ &operator[](int index)
    {
        // RHI_ASSERT(index < elementCount_);
        if (heapDatas_)
        {
            return heapDatas_->at(index);
        }
        else
        {
            return stackDatas_[index];
        }
    }
    
    const Tp_ &operator[](int index) const
    {
        if (heapDatas_)
        {
            return heapDatas_->at(index);
        }
        else
        {
            return stackDatas_[index];
        }
    }
    
    void push_back(const Tp_ &value)
    {
        if (heapDatas_)
        {
            heapDatas_->push_back(value);
            elementCount_++;
            return;
        }
        
        if (elementCount_ < stackDatas_.size())
        {
            stackDatas_[elementCount_++] = value;
            return;
        }
        else
        {
            heapDatas_ = new std::vector<Tp_>(elementCount_ + 1);
            for (size_t i = 0; i < elementCount_; ++i)
            {
                heapDatas_->at(i) = stackDatas_[i];
            }
            heapDatas_->at(elementCount_++) = value;
        }
    }
    
    size_t size() const
    {
        return elementCount_;
    }
    
    void resize(size_t size)
    {
        elementCount_ = size;
        if (heapDatas_)
        {
            heapDatas_->resize(size);
            return;
        }
        if (size > stackDatas_.size())
        {
            heapDatas_ = new std::vector<Tp_>(size);
            for (size_t i = 0; i < stackDatas_.size(); ++i)
            {
                heapDatas_->at(i) = stackDatas_[i];
            }
        }
    }
    
    Tp_* data()
    {
        if (heapDatas_)
        {
            return heapDatas_->data();
        }
        else
        {
            return stackDatas_.data();
        }
    }
    
    const Tp_* data() const
    {
        if (heapDatas_)
        {
            return heapDatas_->data();
        }
        else
        {
            return stackDatas_.data();
        }
    }
    
    void clear()
    {
        if (heapDatas_)
        {
            delete heapDatas_;
            heapDatas_ = nullptr;
        }
        elementCount_ = 0;
    }

protected:
    size_t elementCount_ = 0;
    std::array<Tp_, DefaultSize> stackDatas_;
    std::vector<Tp_>* heapDatas_ = nullptr;
};

NS_RHI_END

#endif //PROJ_ANDROID_TURBOVECTOR_H
