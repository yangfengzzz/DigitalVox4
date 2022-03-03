//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;

kernel void atomicCounter(device atomic_uint* counter[[buffer(0)]],
                          uint3 position [[ thread_position_in_grid ]]) {
    atomic_store_explicit(counter, 0, memory_order::memory_order_relaxed);
    threadgroup_barrier(mem_flags::mem_device);
    
    if (position.x == 0 && position.y == 0 && position.z == 0) {
        atomic_fetch_add_explicit(counter, 1, memory_order::memory_order_relaxed);
    }
}
