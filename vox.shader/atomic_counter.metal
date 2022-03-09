//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;

typedef struct {
    float4 position [[position]];
    float2 v_uv;
} VertexOut;

fragment float4 fragment_atomic(VertexOut in [[stage_in]],
                                constant uint* u_atomic[[buffer(10)]]) {
    uint atomic = u_atomic[0] % 255;
    return float4(atomic/255.0, 1 - atomic/255.0, atomic/255.0, 1.0);
}

kernel void atomicCounter(device atomic_uint* counter[[buffer(0)]],
                          uint3 position [[ thread_position_in_grid ]]) {
//    atomic_store_explicit(counter, 0, memory_order::memory_order_relaxed);
//    threadgroup_barrier(mem_flags::mem_device);
    atomic_fetch_add_explicit(counter, 1, memory_order::memory_order_relaxed);
}
