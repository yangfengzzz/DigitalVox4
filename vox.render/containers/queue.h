//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_CONTAINERS_QUEUE_H_
#define VOX_VOX_BASE_CONTAINERS_QUEUE_H_

#include <queue>

#include "deque.h"

namespace vox {
// Redirects std::queue to vox::queue in order to replace std default allocator
// by vox::StdAllocator.
template <class _Ty, class _Container = deque<_Ty>>
using queue = std::queue<_Ty, _Container>;

// Redirects std::priority_queue to vox::priority_queue in order to replace std
// default allocator by vox::StdAllocator.
template <class _Ty, class _Container = deque<_Ty>,
class _Pred = std::less<typename _Container::value_type>>
using priority_queue = std::priority_queue<_Ty, _Container, _Pred>;
}  // namespace vox
#endif  // VOX_VOX_BASE_CONTAINERS_QUEUE_H_
