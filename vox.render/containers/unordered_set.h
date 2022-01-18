//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_CONTAINERS_UNORDERED_SET_H_
#define VOX_VOX_BASE_CONTAINERS_UNORDERED_SET_H_

#include <unordered_set>

#include "containers/std_allocator.h"

namespace vox {
// Redirects std::unordered_set to vox::UnorderedSet in order to replace std
// default allocator by vox::StdAllocator.
template <class _Key, class _Hash = std::hash<_Key>,
class _KeyEqual = std::equal_to<_Key>,
class _Allocator = vox::StdAllocator<_Key> >
using unordered_set = std::unordered_set<_Key, _Hash, _KeyEqual, _Allocator>;

// Redirects std::unordered_multiset to vox::UnorderedMultiSet in order to
// replace std default allocator by vox::StdAllocator.
template <class _Key, class _Hash = std::hash<_Key>,
class _KeyEqual = std::equal_to<_Key>,
class _Allocator = vox::StdAllocator<_Key> >
using unordered_multiset = std::unordered_multiset<_Key, _Hash, _KeyEqual, _Allocator>;
}  // namespace vox
#endif  // VOX_VOX_BASE_CONTAINERS_UNORDERED_SET_H_
