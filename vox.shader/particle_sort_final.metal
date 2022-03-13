//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_config.h"

kernel void particle_sort_final(device uint* indices [[buffer(0)]],
                                device TParticle* read_particles [[buffer(7)]],
                                device TParticle* write_particles [[buffer(8)]],
                                uint gid [[ thread_position_in_grid ]]) {
    uint read_id = indices[gid];
    write_particles[gid] = read_particles[read_id];    
}
