//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_VOX_BASE_MEMORY_UNIQUE_PTR_H_
#define VOX_VOX_BASE_MEMORY_UNIQUE_PTR_H_

#include "memory/allocator.h"

#include <memory>
#include <utility>

namespace vox {

// Defaut deleter for vox unique_ptr, uses redirected memory allocator.
template <typename _Ty>
struct Deleter {
    Deleter() {}
    
    template <class _Up>
    Deleter(const Deleter<_Up>&, _Ty* = nullptr) {}
    
    void operator()(_Ty* _ptr) const {
        vox::Delete(_ptr);
    }
};

// Defines vox::unique_ptr to use vox default deleter.
template <typename _Ty, typename _Deleter = vox::Deleter<_Ty>>
using unique_ptr = std::unique_ptr<_Ty, _Deleter>;

// Implements make_unique to use vox redirected memory allocator.
template <typename _Ty, typename... _Args>
unique_ptr<_Ty> make_unique(_Args&&... _args) {
    return unique_ptr<_Ty>(New<_Ty>(std::forward<_Args>(_args)...));
}

}  // namespace vox
#endif  // VOX_VOX_BASE_MEMORY_UNIQUE_PTR_H_
