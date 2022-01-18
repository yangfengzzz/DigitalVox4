//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_CONTAINERS_DEQUE_H_
#define VOX_VOX_BASE_CONTAINERS_DEQUE_H_

#include <deque>

#include "containers/std_allocator.h"

namespace vox {
// Redirects std::deque to vox::deque in order to replace std default allocator
// by vox::StdAllocator.
template <class _Ty, class _Allocator = vox::StdAllocator<_Ty>>
using deque = std::deque<_Ty, _Allocator>;

}  // namespace vox
#endif  // VOX_VOX_BASE_CONTAINERS_DEQUE_H_
