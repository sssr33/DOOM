#pragma once

#include <memory>

struct AddRefKey {};
struct DoNotAddRefKey {};

template<class T>
class InterfacePtr {
public:
    InterfacePtr() = default;
    InterfacePtr(T* iptr, AddRefKey)
        : ptr(iptr)
    {
        this->AddRef();
    }

    InterfacePtr(T* iptr, DoNotAddRefKey)
        : ptr(iptr)
    {}

    InterfacePtr(const InterfacePtr& other)
        : ptr(other.get())
    {
        this->AddRef();
    }

    InterfacePtr(InterfacePtr&& other)
        : ptr(std::move(other.ptr))
    {}

    InterfacePtr& operator=(const InterfacePtr& other) {
        if (this == &other) {
            return *this;
        }

        this->ptr.reset(other.get());
        this->AddRef();

        return *this;
    }

    InterfacePtr& operator=(InterfacePtr&& other) {
        if (this == &other) {
            return *this;
        }

        this->ptr = std::move(other.ptr);

        return *this;
    }

    explicit operator bool() const {
        return this->ptr != nullptr;
    }

    T* get() const {
        return this->ptr.get();
    }

    T* detach() {
        return this->ptr.release();
    }

    T* operator->() const {
        return this->ptr.get();
    }

private:
    struct Deleter {
        void operator()(T* iptr) {
            iptr->vtable->Release(iptr);
        }
    };

    void AddRef() {
        if (!this->ptr) {
            return;
        }

        this->ptr->vtable->AddRef(ptr.get());
    }

    std::unique_ptr<T, Deleter> ptr;
};
