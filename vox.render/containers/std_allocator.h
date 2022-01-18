//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_CONTAINERS_STD_ALLOCATOR_H_
#define VOX_VOX_BASE_CONTAINERS_STD_ALLOCATOR_H_

#include <new>

#include "memory/allocator.h"

namespace vox {
// Define a STL allocator compliant allocator->
template <typename _Ty>
class StdAllocator {
public:
    typedef _Ty value_type;                     // Element type.
    typedef value_type* pointer;                // Pointer to element.
    typedef value_type& reference;              // Reference to element.
    typedef const value_type* const_pointer;    // Constant pointer to element.
    typedef const value_type& const_reference;  // Constant reference to element.
    typedef size_t size_type;                   // Quantities of elements.
    typedef ptrdiff_t difference_type;  // Difference between two pointers.
    
    StdAllocator() noexcept {}
    StdAllocator(const StdAllocator&) noexcept {}
    
    template <class _Other>
    StdAllocator<value_type>(const StdAllocator<_Other>&) noexcept {}
    
    template <class _Other>
    struct rebind {
        typedef StdAllocator<_Other> other;
    };
    
    pointer address(reference _ref) const noexcept { return &_ref; }
    const_pointer address(const_reference _ref) const noexcept { return &_ref; }
    
    template <class _Other, class... _Args>
    void construct(_Other* _ptr, _Args&&... _args) {
        ::new (static_cast<void*>(_ptr)) _Other(std::forward<_Args>(_args)...);
    }
    
    template <class _Other>
    void destroy(_Other* _ptr) {
        (void)_ptr;
        _ptr->~_Other();
    }
    
    // Allocates array of _Count elements.
    pointer allocate(size_t _count) noexcept {
        // Makes sure to a use c like allocator, to avoid duplicated constructor
        // calls.
        return reinterpret_cast<pointer>(memory::default_allocator()->Allocate(sizeof(value_type) * _count, alignof(value_type)));
    }
    
    // Deallocates object at _Ptr, ignores size.
    void deallocate(pointer _ptr, size_type) noexcept {
        memory::default_allocator()->Deallocate(_ptr);
    }
    
    size_type max_size() const noexcept {
        return (~size_type(0)) / sizeof(value_type);
    }
};

// Tests for allocator equality (always true).
template <class _Ty, class _Other>
inline bool operator==(const StdAllocator<_Ty>&,
                       const StdAllocator<_Other>&) noexcept {
    return true;
}

// Tests for allocator inequality (always false).
template <class _Ty, class _Other>
inline bool operator!=(const StdAllocator<_Ty>&,
                       const StdAllocator<_Other>&) noexcept {
    return false;
}

}  // namespace vox
#endif  // VOX_VOX_BASE_CONTAINERS_STD_ALLOCATOR_H_
