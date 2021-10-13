#ifndef SHAREDPTR_H
#define SHAREDPTR_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility>
#include <atomic>
#include <typeinfo>
#include <mutex>
#include <cstddef>
#include <type_traits>

//Used so I can create an empty block pointer
class BlockPointer {
public:
    std::atomic<int> counter;
    BlockPointer() {
        this->counter = 0;
    }
    virtual ~BlockPointer() {}
};

//controlBlock - holds the actual data and the ref counter inside of it
template <typename T> class ControlBlock : public BlockPointer {
public:
    T* data;
    //Constructor
    ControlBlock() {
        this->data = NULL;
        this->counter = 0;
    }
    //Copy Con
    ControlBlock(T* newData) {
        this->data = newData;
        this->counter++;
    }
    //Deconstructor
    ~ControlBlock() {
        delete this->data;
        this->data = NULL;
        this->counter = 0;
    }
};

template <typename T> class TSharedPtr {

public:
    //ControlBlock that stores the data and the reference counter
    //ControlBlock<T> * cBlock;
    BlockPointer* cBlock;
    //The data itself
    T* data;


    //CONSTRUCTORS, ASSINGMENTS , DECONSTRUCTOR
    //Constructs a smart pointer that points to NUll
    TSharedPtr() {
        this->data = NULL;
        this->cBlock = NULL;
    }

    //Constructor to an obj
    template <typename U> explicit TSharedPtr(U* obj) {
        this->cBlock = new ControlBlock<U>(obj);
        this->data = static_cast<T*>(obj);

    }


    //If p is not null, then reference count of the managed object is incremented. 
    TSharedPtr(const TSharedPtr& p) {

        this->data = p.data;
        this->cBlock = p.cBlock;
        //If p is not Null increment the counter
        if (this->cBlock != NULL) {
            this->cBlock->counter++;
        }

    }

    //If U * is not implicitly convertible to T *, use of the second constructor will result in a compile-time error when the compiler attempts to instantiate the member template.
    template <typename U> TSharedPtr(const TSharedPtr<U>& p) {

        this->data = static_cast<T*>(p.data);
        this->cBlock = (ControlBlock<T>*)p.cBlock;
        if (this->cBlock != NULL) {
            this->cBlock->counter++;
        }
        
    }

    //Move the managed object from the given smart pointer. The reference count must remain unchanged. After this function, p must be null. This must work if U * is implicitly convertible to T *
    TSharedPtr(TSharedPtr&& p) {

        this->data = p.data;
        this->cBlock = p.cBlock;
        p.data = NULL;
        p.cBlock = NULL;

    }

    template <typename U> TSharedPtr(TSharedPtr<U>&& p) {

        this->data = static_cast<T*>(p.data);
        this->cBlock = (ControlBlock<T>*)p.cBlock;
        p.data = NULL;
        p.cBlock = NULL;

    }

    //Copy Assignment
    TSharedPtr& operator=(const TSharedPtr& shrPtr) {
        //If we are not self assigning

        if (this != &shrPtr) {
            // pthread_mutex_lock(&counter_mutex);
            //Clearing this pointers connection to the old data if it was the last pointer
            if (this->cBlock != NULL) {
                //If it was the last pointer we clear the controlBlock
                if (this->cBlock->counter == 1 || this->cBlock->counter == 0) {
                    this->cBlock->counter = 0;
                    delete (this->cBlock);
                    this->cBlock = NULL;
                    this->data = NULL;
                }
                else {
                    this->cBlock->counter--;
                }
                //If it isnt the last pointer the cBlock shouldnt be deleted as something still points to it
            }

            //Set the data
            this->data = shrPtr.data;
            this->cBlock = shrPtr.cBlock;
            if (this->cBlock != NULL) {
                this->cBlock->counter++;
            }
            //  pthread_mutex_unlock(&counter_mutex);
        }

        return *this;
    }

    template <typename U>TSharedPtr<T>& operator=(const TSharedPtr<U>& shrPtr) {
        //If we are not self assigning 

        if (this != ((TSharedPtr<T>*) & shrPtr)) {
            // pthread_mutex_lock(&counter_mutex);
            //Clearing this pointers connection to the old data if it was the last pointer
            if (this->cBlock != NULL) {
                //If it was the last pointer we clear the controlBlock
                if (this->cBlock->counter == 1 || this->cBlock->counter == 0) {
                    this->cBlock->counter = 0;
                    delete (this->cBlock);
                    this->cBlock = NULL;
                    this->data = NULL;
                }
                else {
                    this->cBlock->counter--;
                }
                //If it isnt the last pointer the cBlock shouldnt be deleted as something still points to it
            }

            //Set the data
            this->data = static_cast<T*>(shrPtr.data);
            this->cBlock = (ControlBlock<T>*)shrPtr.cBlock;
            if (this->cBlock != NULL) {
                this->cBlock->counter++;
            }
            //  pthread_mutex_unlock(&counter_mutex);
        }

        return *this;
    }

    //Move assignments
    TSharedPtr& operator=(TSharedPtr&& p) {
        //We moving a non empty pointer

        if (p.cBlock != NULL) {
            // pthread_mutex_lock(&counter_mutex);
            //Check to see if we need update controlBlock
            if (this->cBlock != NULL) {
                //If its the last element gotta clear cBlock before we update
                if (this->cBlock->counter == 1 || this->cBlock->counter == 0) {
                    this->cBlock->counter = 0;
                    delete this->cBlock;
                    this->cBlock = NULL;
                    this->data = NULL;
                }
                else {
                    this->cBlock->counter--;
                }
            }
            this->data = p.data;
            this->cBlock = p.cBlock;

            //Clear p
            p.data = NULL;
            p.cBlock = NULL;

            //  pthread_mutex_unlock(&counter_mutex);
        }
        //Else we are copying an empty pointer so p is already clear
        else {

            this->data = NULL;
            this->cBlock = NULL;

        }

        return *this;
    }
    template <typename U> TSharedPtr& operator=(TSharedPtr<U>&& p) {
        //We moving a non empty pointer

        if (p.cBlock != NULL) {
            // pthread_mutex_lock(&counter_mutex);
            //Check to see if we need update controlBlock
            if (this->cBlock != NULL) {
                //If its the last element gotta clear cBlock before we update
                if (this->cBlock->counter == 1 || this->cBlock->counter == 0) {
                    this->cBlock->counter = 0;
                    delete this->cBlock;
                    this->cBlock = NULL;
                    this->data = NULL;
                }
                else {
                    this->cBlock->counter--;
                }
            }
            this->data = static_cast<T*>(p.data);
            this->cBlock = (ControlBlock<T>*)p.cBlock;
            //Clear p
            p.data = NULL;
            p.cBlock = NULL;
            //  pthread_mutex_unlock(&counter_mutex);

        }
        //Else we are copying an empty pointer so p is already clear
        else {

            this->data = NULL;
            this->cBlock = NULL;

        }

        return *this;
    }

    //Deconstructor- Decrement reference count of managed object. If the reference count is zero, delete the object.
    ~TSharedPtr() {
        //If its not already empty (otherwise we done)
        // pthread_mutex_lock(&counter_mutex);
        if (this->cBlock != NULL) {

            //If count will be 0 we gotta clear it
            if (this->cBlock->counter == 1 || this->cBlock->counter == 0) {
                this->cBlock->counter = 0;
                delete this->cBlock;
                this->cBlock = NULL;
                this->data = NULL;
            }
            //Else we decrement count
            else {
                this->cBlock->counter--;
            }

        }
        //  pthread_mutex_unlock(&counter_mutex);
    }


    //MODIFIERS

    //The smart pointer is set to point to the null pointer. The reference count for the currently pointed to object, if any, is decremented.
    void reset() {
        //It is not empty
        // pthread_mutex_lock(&counter_mutex);
        if (this->cBlock != NULL) {

            //Case where its the last pointer to a box
            if (this->cBlock->counter == 1 || this->cBlock->counter == 0) {
                this->cBlock->counter = 0;
                delete this->cBlock;
                this->cBlock = NULL;
                this->data = NULL;
            }
            //Else we dec
            else {
                //This might be wrong?
                this->cBlock->counter--;
                this->cBlock = NULL;
                this->data = NULL;
            }

        }
        //  pthread_mutex_unlock(&counter_mutex);
    }

    //Replace owned resource with another pointer. If the owned resource has no other references, it is deleted. If p has been associated with some other smart pointer, the behavior is undefined.
    template <typename U> void reset(U* p) {
        //It is not empty

        if (this->cBlock != NULL) {
            // pthread_mutex_lock(&counter_mutex);
            //Case where its the last pointer to a box
            if (this->cBlock->counter == 1 || this->cBlock->counter == 0) {
                this->cBlock->counter = 0;
                delete this->cBlock;
                this->cBlock = NULL;
                this->data = NULL;
            }
            //Else we dec
            else {
                //This might be wrong?
                this->cBlock->counter--;
                this->cBlock = NULL;
                this->data = NULL;
            }
            //  pthread_mutex_unlock(&counter_mutex);
        }
        //WHY IS THIS U MAYBE SHOULD BE T?
        this->cBlock = new ControlBlock<U>(p);
        this->data = static_cast<T*>(p);

    }
    //OBSERVERS

    //Returns a pointer to the owned object. Note that this will be a pointer-to-const if T is a const-qualified type.
    T* get() const {
        return (this->data);
    }

    //A reference to the pointed-to object is returned. Note that this will be a const-reference if T is a const-qualified type.
    T& operator*() const {
        return *(this->data);
    }

    //The pointer is returned. Note that this will be a pointer-to-const if T is a const-qualified type.
    T* operator->() const {
        return this->data;
    }

    //Returns true if the SharedPtr is not null.
    explicit operator bool() const {
        if (this->data == NULL) {
            return false;
        }
        else {
            return true;
        }
    }



};

//FREE FUNCTIONS


template <typename T1, typename T2>
bool operator==(const TSharedPtr<T1>& shrPtr1, const TSharedPtr<T2>& shrPtr2) {
    if (((shrPtr1.data == NULL) && (shrPtr2.data == NULL)) || (shrPtr1.data == shrPtr2.data)) {
        return true;
    }
    else {
        return false;
    }
}

template <typename T>
bool operator==(const TSharedPtr<T>& shrPtr, std::nullptr_t nullP) {
    if (shrPtr.data == nullP) {
        return true;
    }
    else {
        return false;
    }
}

template <typename T>
bool operator==(std::nullptr_t nullP, const TSharedPtr<T>& shrPtr) {
    if (shrPtr.data == nullP) {
        return true;
    }
    else {
        return false;
    }
}

template <typename T1, typename T2>
bool operator!=(const TSharedPtr<T1>& shrPtr1, const TSharedPtr<T2>& shrPtr2) {
    if (((shrPtr1.data == NULL) && (shrPtr2.data == NULL)) || (shrPtr1.data == shrPtr2.data)) {
        return false;
    }
    else {
        return true;
    }
}

template <typename T>
bool operator!=(const TSharedPtr<T>& shrPtr, std::nullptr_t nullP) {
    if (shrPtr.data == nullP) {
        return false;
    }
    else {
        return true;
    }
}

template <typename T>
bool operator!=(std::nullptr_t nullP, const TSharedPtr<T>& shrPtr) {

    if (shrPtr.data == nullP) {
        return false;
    }
    else {
        return true;
    }
}

template <typename T, typename U>
TSharedPtr<T> static_pointer_cast(const TSharedPtr<U>& sp) {
    TSharedPtr<T> shrPtr(sp);
    shrPtr.data = static_cast<T*>(sp.data);
    return shrPtr;
}

template <typename T, typename U>
TSharedPtr<T> dynamic_pointer_cast(const TSharedPtr<U>& sp) {
    TSharedPtr<T> shrPtr(sp);
    shrPtr.data = dynamic_cast<T*>(sp.data);
    return shrPtr;

}


#endif

