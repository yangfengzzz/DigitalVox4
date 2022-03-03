//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_config.h"

kernel void particle_sort_final(device uint* indices [[buffer(0)]],
#if USE_SOA_LAYOUT
                                device float4* read_positions [[buffer(1)]],
                                device float4* read_velocities [[buffer(2)]],
                                device float4* read_attributes [[buffer(3)]],
                                device float4* write_positions [[buffer(4)]],
                                device float4* write_velocities [[buffer(5)]],
                                device float4* write_attributes [[buffer(6)]],
#else
                                device TParticle* read_particles [[buffer(7)]],
                                device TParticle* write_particles [[buffer(8)]],
#endif
                                uint gid [[ thread_position_in_grid ]]) {
    uint read_id = indices[gid];
    
#if USE_SOA_LAYOUT
    write_positions[gid]  = read_positions[read_id];
    write_velocities[gid] = read_velocities[read_id];
    write_attributes[gid] = read_attributes[read_id];
#else
    write_particles[gid] = read_particles[read_id];
#endif
    
}
