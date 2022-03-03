//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_config.h"

TParticle PopParticle(device TParticle* read_particles,
                      device atomic_uint* read_count,
#if SPARKLE_USE_SOA_LAYOUT
                      device float4* read_positions,
                      device float4* read_velocities,
                      device float4* read_attributes,
#endif
                      uint index) {
    atomic_fetch_sub_explicit(read_count, 1, memory_order::memory_order_relaxed);
    
    TParticle p;
    
#if SPARKLE_USE_SOA_LAYOUT
    p.position   = read_positions[index];
    p.velocity   = read_velocities[index];
    float4 attribs = read_attributes[index];
    
    p.start_age  = attribs.x;
    p.age        = attribs.y;
    p.id         = as_type<uint>(attribs.w);
#else
    p = read_particles[index];
#endif
    
    return p;
}

void PushParticle(TParticle p
                  , device atomic_uint* write_count
#if SPARKLE_USE_SOA_LAYOUT
                  , device float4* write_positions
                  , device float4* write_velocities
                  , device float4* write_attributes
#else
                  , device TParticle* write_particles
#endif
                  ) {
    const uint index = atomic_fetch_add_explicit(write_count, 1, memory_order::memory_order_relaxed);
    
#if SPARKLE_USE_SOA_LAYOUT
    write_positions[index]  = p.position;
    write_velocities[index] = p.velocity;
    write_attributes[index] = float4(p.start_age, p.age, 0.0f, as_type<float>(p.id));
#else
    write_particles[index] = p;
#endif
}
