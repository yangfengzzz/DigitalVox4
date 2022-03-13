//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_math.h"

void pushParticle(float3 position,
                  float3 velocity,
                  float age,
                  device TParticle* particles,
                  device atomic_uint* write_count) {
    // Emit particle id.
    const uint id = atomic_fetch_add_explicit(write_count, 1, memory_order::memory_order_relaxed);
    
    TParticle p;
    p.position = float4(position, 1.0f);
    p.velocity = float4(velocity, 0.0f);
    p.start_age = age;
    p.age = age;
    p.id = id;
    
    particles[id] = p;
}

void createParticle(uint uEmitCount,
                    uint uEmitterType,
                    float3 uEmitterPosition,
                    float3 uEmitterDirection,
                    float uEmitterRadius,
                    float uParticleMinAge,
                    float uParticleMaxAge,
                    device atomic_uint* write_count,
                    device TParticle* particles,
                    device float* randbuffer,
                    const uint gid) {
    // Random vector.
    const uint rid = 3u * gid;
    const float3 rn = float3(randbuffer[rid], randbuffer[rid+1u], randbuffer[rid+2u]);
    
    // Position
    float3 pos = uEmitterPosition;
    if (uEmitterType == 1) {
        //pos += disk_distribution(uEmitterRadius, rn.xy);
        pos += disk_even_distribution(uEmitterRadius, gid, uEmitCount);
    } else if (uEmitterType == 2) {
        pos += sphere_distribution(uEmitterRadius, rn.xy);
    } else if (uEmitterType == 3) {
        pos += ball_distribution(uEmitterRadius, rn);
    }
    
    // Velocity
    float3 vel = uEmitterDirection;
    
    // Age
    // The age is set by thread groups to assure we have a number of particles
    // factors of groupWidth, this method is safe but prevents continuous emission.
    // const float group_rand = randbuffer[gid];
    // [As the threadgroup are not full, some dead particles might appears if not
    // skipped in following stages].
    const float single_rand = randbuffer[gid];
    
    const float age = mix( uParticleMinAge, uParticleMaxAge, single_rand);
    
    pushParticle(pos, vel, age,
                 particles,
                 write_count);
}


kernel void particle_emission(constant uint& uEmitCount [[buffer(0)]],
                              constant uint& uEmitterType [[buffer(1)]],
                              constant float3& uEmitterPosition [[buffer(2)]],
                              constant float3& uEmitterDirection [[buffer(3)]],
                              constant float& uEmitterRadius [[buffer(4)]],
                              constant float& uParticleMinAge [[buffer(5)]],
                              constant float& uParticleMaxAge [[buffer(6)]],
                              device atomic_uint* write_count [[buffer(7)]],
                              device TParticle* particles [[buffer(11)]],
                              device float* randbuffer [[buffer(12)]],
                              const uint gid [[ thread_position_in_grid ]]) {    
    createParticle(uEmitCount,
                   uEmitterType,
                   uEmitterPosition,
                   uEmitterDirection,
                   uEmitterRadius,
                   uParticleMinAge,
                   uParticleMaxAge,
                   write_count,
                   particles,
                   randbuffer,
                   gid);
}
