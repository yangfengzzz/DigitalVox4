//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_CONTAINERS_LIST_H_
#define VOX_VOX_BASE_CONTAINERS_LIST_H_

#ifdef _MSC_VER
#pragma warning(push)
// Removes constant conditional expression warning.
#pragma warning(disable : 4127)
#endif  // _MSC_VER

#include <list>

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#include "containers/std_allocator.h"

namespace vox {
// Redirects std::list to vox::list in order to replace std default allocator by
// vox::StdAllocator.
template <class _Ty, class _Allocator = vox::StdAllocator<_Ty>>
using list = std::list<_Ty, _Allocator>;
}  // namespace vox
#endif  // VOX_VOX_BASE_CONTAINERS_LIST_H_
