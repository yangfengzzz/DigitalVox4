/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Default free store allocator template for C++ Metal wrapper
*/

#ifndef CPPMetalAllocator_hpp
#define CPPMetalAllocator_hpp
#include <new>

namespace MTL
{

// Memory Managment Notes:
//
// Any object returned by CPPMetal methods whose name begins 'new' is responsibility of the caller
// to destroy with MTL::destroy().  Also, the Device created with MTL::CreateSystemDefaultDevice is
// is also the responsiblity of the caller to destroy with MTL::destroy().
//
// CPPMetal manages the memory of any other object and will destroy it when necessary.

class Allocator
{
public:

    virtual void* allocate(size_t size);
    virtual void deallocate(void* ptr);

    Allocator()                            = default;
    Allocator(const Allocator&)            = default;
    Allocator(Allocator&&)                 = default;
    Allocator& operator=(const Allocator&) = default;
    Allocator& operator=(Allocator&&)      = default;
    virtual ~Allocator();

    template<typename ConstructionType, typename... ConstructionArgs>
    ConstructionType *construct(ConstructionArgs&&... args)
    {
        void * mem = allocate(sizeof(ConstructionType));
        return new (mem) ConstructionType(args...);
    }

    template<typename ConstructionType>
    void destroy(ConstructionType *ptr)
    {
        if(ptr)
        {
            ptr->~ConstructionType();
        }
        deallocate(ptr);
    }

};

inline void* Allocator::allocate(size_t size)
{
    return ::operator new(size);
}

inline void Allocator::deallocate(void* p)
{
    ::operator delete(p);
}

inline Allocator::~Allocator()
{

}

template <typename T, typename... ConstructionArgs>
static inline T* construct(Allocator & allocator, ConstructionArgs&&... args)
{
    return allocator.construct<T>(args...);
}

template <typename T>
static inline void destroy(Allocator & allocator, T *ptr)
{
    allocator.destroy(ptr);
}

} // namespace MTL

#endif // CPPMetalAllocator_hpp
