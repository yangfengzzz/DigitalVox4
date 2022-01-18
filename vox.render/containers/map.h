//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_CONTAINERS_MAP_H_
#define VOX_VOX_BASE_CONTAINERS_MAP_H_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)  // warning C4702: unreachable code
#endif                           // _MSC_VER

#include <cstring>
#include <map>

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#include "containers/std_allocator.h"

namespace vox {
// Redirects std::map to vox::map in order to replace std default allocator by
// vox::StdAllocator.
template <class _Key, class _Ty, class _Pred = std::less<_Key>,
class _Allocator = vox::StdAllocator<std::pair<const _Key, _Ty>>>
using map = std::map<_Key, _Ty, _Pred, _Allocator>;

// Implements a string comparator that can be used by std algorithm like maps.
struct str_less {
    bool operator()(const char* const& _left, const char* const& _right) const {
        return strcmp(_left, _right) < 0;
    }
};

// Specializes std::map to use c-string as a key.
template <class _Ty, class _Allocator =
vox::StdAllocator<std::pair<const char* const, _Ty>>>
using cstring_map = std::map<const char*, _Ty, str_less, _Allocator>;

// Redirects std::multimap to vox::MultiMap in order to replace std default
// allocator by vox::StdAllocator.
template <class _Key, class _Ty, class _Pred = std::less<_Key>,
class _Allocator = vox::StdAllocator<std::pair<const _Key, _Ty>>>
using multimap = std::multimap<_Key, _Ty, _Pred, _Allocator>;
}  // namespace vox
#endif  // VOX_VOX_BASE_CONTAINERS_MAP_H_
