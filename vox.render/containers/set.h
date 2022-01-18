//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_CONTAINERS_SET_H_
#define VOX_VOX_BASE_CONTAINERS_SET_H_

#include <set>

#include "containers/std_allocator.h"

namespace vox {
// Redirects std::set to vox::set in order to replace std default allocator by
// vox::StdAllocator.
template <class _Key, class _Pred = std::less<_Key>,
class _Allocator = vox::StdAllocator<_Key>>
using set = std::set<_Key, _Pred, _Allocator>;

// Redirects std::multiset to vox::multiset in order to replace std default
// allocator by vox::StdAllocator.
template <class _Key, class _Pred = std::less<_Key>,
class _Allocator = vox::StdAllocator<_Key>>
using multiset = std::multiset<_Key, _Pred, _Allocator>;
}  // namespace vox
#endif  // VOX_VOX_BASE_CONTAINERS_SET_H_
