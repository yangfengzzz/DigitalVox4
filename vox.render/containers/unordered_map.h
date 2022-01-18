//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_CONTAINERS_UNORDERED_MAP_H_
#define VOX_VOX_BASE_CONTAINERS_UNORDERED_MAP_H_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)  // warning C4702: unreachable code
#endif                           // _MSC_VER

#include <unordered_map>

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#include "containers/std_allocator.h"

namespace vox {

// Redirects std::unordered_map to vox::unordered_map in order to replace std
// default allocator by vox::StdAllocator.
template <class _Key, class _Ty, class _Hash = std::hash<_Key>,
class _KeyEqual = std::equal_to<_Key>,
class _Allocator = vox::StdAllocator<std::pair<const _Key, _Ty>>>
using unordered_map = std::unordered_map<_Key, _Ty, _Hash, _KeyEqual, _Allocator>;

// Redirects std::unordered_multimap to vox::UnorderedMultiMap in order to
// replace std default allocator by vox::StdAllocator.
template <class _Key, class _Ty, class _Hash = std::hash<_Key>,
class _KeyEqual = std::equal_to<_Key>,
class _Allocator = vox::StdAllocator<std::pair<const _Key, _Ty>>>
using unordered_multimap = std::unordered_multimap<_Key, _Ty, _Hash, _KeyEqual, _Allocator>;
}  // namespace vox
#endif  // VOX_VOX_BASE_CONTAINERS_UNORDERED_MAP_H_
