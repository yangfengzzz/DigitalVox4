//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;

bool cmp(float a, float b) {
    return a > b;
}

void compareAndSwap(uint order, thread uint& left, thread uint& right,
                    device float* dp) {
    if (bool(order) == cmp(dp[left], dp[right])) {
        left  ^= right;
        right ^= left;
        left  ^= right;
    }
}

kernel void particle_sort_step(constant uint& uBlockWidth [[buffer(0)]],
                               constant uint& uMaxBlockWidth [[buffer(1)]],
                               device uint* read_indices [[buffer(2)]],
                               device uint* write_indices [[buffer(3)]],
                               device float* dp [[buffer(4)]],
                               uint gid [[ thread_position_in_grid ]]) {
    const uint max_block_width = uMaxBlockWidth;
    const uint block_width = uBlockWidth;
    const uint pair_distance = block_width / 2u;
    
    const uint block_offset = (gid / pair_distance) * block_width;
    const uint left_id = block_offset + (gid % pair_distance);
    const uint right_id = left_id + pair_distance;
    
    // Data to sort
    uint left_data  = read_indices[left_id];
    uint right_data = read_indices[right_id];
    
    const uint order = (left_id / max_block_width) & 1;
    compareAndSwap(order, left_data, right_data, dp);
    
    write_indices[left_id]  = left_data;
    write_indices[right_id] = right_data;
}
