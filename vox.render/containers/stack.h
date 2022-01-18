//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_CONTAINERS_STACK_H_
#define VOX_VOX_BASE_CONTAINERS_STACK_H_

#include <stack>

#include "deque.h"

namespace vox {
// Redirects std::stack to vox::stack in order to replace std default allocator
// by vox::StdAllocator.
template <class _Ty, class _Container = typename vox::deque<_Ty>>
using stack = std::stack<_Ty, _Container>;
}  // namespace vox
#endif  // VOX_VOX_BASE_CONTAINERS_STACK_H_
