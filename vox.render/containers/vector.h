//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_CONTAINERS_VECTOR_H_
#define VOX_VOX_BASE_CONTAINERS_VECTOR_H_

#include <vector>

#include "containers/std_allocator.h"

namespace vox {
// Redirects std::vector to vox::vector in order to replace std default
// allocator by vox::StdAllocator.
template <class _Ty, class _Allocator = vox::StdAllocator<_Ty>>
using vector = std::vector<_Ty, _Allocator>;

// Extends std::vector with two functions that gives access to the begin and the
// end of its array of elements.

// Returns the mutable begin of the array of elements, or nullptr if
// vector's empty.
template <class _Ty, class _Allocator>
inline _Ty* array_begin(std::vector<_Ty, _Allocator>& _vector) {
    return _vector.data();
}

// Returns the non-mutable begin of the array of elements, or nullptr if
// vector's empty.
template <class _Ty, class _Allocator>
inline const _Ty* array_begin(const std::vector<_Ty, _Allocator>& _vector) {
    return _vector.data();
}

// Returns the mutable end of the array of elements, or nullptr if
// vector's empty. Array end is one element past the last element of the
// array, it cannot be dereferenced.
template <class _Ty, class _Allocator>
inline _Ty* array_end(std::vector<_Ty, _Allocator>& _vector) {
    return _vector.data() + _vector.size();
}

// Returns the non-mutable end of the array of elements, or nullptr if
// vector's empty. Array end is one element past the last element of the
// array, it cannot be dereferenced.
template <class _Ty, class _Allocator>
inline const _Ty* array_end(const std::vector<_Ty, _Allocator>& _vector) {
    return _vector.data() + _vector.size();
}

}  // namespace vox
#endif  // VOX_VOX_BASE_CONTAINERS_VECTOR_H_
