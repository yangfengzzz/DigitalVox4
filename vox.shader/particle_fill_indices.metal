//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;

kernel void particle_fill_indices(device uint* indices [[buffer(0)]],
                                  uint gid [[ thread_position_in_grid ]]) {
    indices[gid] = gid;
}
