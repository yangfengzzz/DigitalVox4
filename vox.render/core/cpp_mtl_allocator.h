//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_Allocator_hpp
#define cpp_mtl_Allocator_hpp

#include <new>

namespace MTL {
// Memory Managment Notes:
//
// Any object returned by cpp_mtl_ methods whose name begins 'new' is responsibility of the caller
// to destroy with MTL::destroy().  Also, the Device created with MTL::CreateSystemDefaultDevice is
// is also the responsiblity of the caller to destroy with MTL::destroy().
//
// cpp_mtl_ manages the memory of any other object and will destroy it when necessary.
class Allocator {
public:
    virtual void *allocate(size_t size);
    
    virtual void deallocate(void *ptr);
    
    Allocator() = default;
    
    Allocator(const Allocator &) = default;
    
    Allocator(Allocator &&) = default;
    
    Allocator &operator=(const Allocator &) = default;
    
    Allocator &operator=(Allocator &&) = default;
    
    virtual ~Allocator();
    
    template<typename ConstructionType, typename... ConstructionArgs>
    ConstructionType *construct(ConstructionArgs &&... args) {
        void *mem = allocate(sizeof(ConstructionType));
        return new(mem) ConstructionType(args...);
    }
    
    template<typename ConstructionType>
    void destroy(ConstructionType *ptr) {
        if (ptr) {
            ptr->~ConstructionType();
        }
        deallocate(ptr);
    }
};

inline void *Allocator::allocate(size_t size) {
    return ::operator new(size);
}

inline void Allocator::deallocate(void *p) {
    ::operator delete(p);
}

inline Allocator::~Allocator() {
    
}

template<typename T, typename... ConstructionArgs>
static inline T *construct(Allocator &allocator, ConstructionArgs &&... args) {
    return allocator.construct<T>(args...);
}

template<typename T>
static inline void destroy(Allocator &allocator, T *ptr) {
    allocator.destroy(ptr);
}

} // namespace MTL

#endif // cpp_mtl_Allocator_hpp
