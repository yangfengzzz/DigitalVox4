//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_curl_noise.h"
#include "function-constant.h"

TParticle popParticle(device atomic_uint* read_count,
#if USE_SOA_LAYOUT
                      device float4* read_positions,
                      device float4* read_velocities,
                      device float4* read_attributes,
#else
                      device TParticle* read_particles,
#endif
                      uint index) {
    atomic_fetch_sub_explicit(read_count, 1, memory_order::memory_order_relaxed);
    
    TParticle p;
    
#if USE_SOA_LAYOUT
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

void pushParticle(TParticle p
                  , device atomic_uint* write_count
#if USE_SOA_LAYOUT
                  , device float4* write_positions
                  , device float4* write_velocities
                  , device float4* write_attributes
#else
                  , device TParticle* write_particles
#endif
                  ) {
    const uint index = atomic_fetch_add_explicit(write_count, 1, memory_order::memory_order_relaxed);
    
#if USE_SOA_LAYOUT
    write_positions[index]  = p.position;
    write_velocities[index] = p.velocity;
    write_attributes[index] = float4(p.start_age, p.age, 0.0f, as_type<float>(p.id));
#else
    write_particles[index] = p;
#endif
}

float updatedAge(TParticle p,
                 float uTimeStep) {
    return clamp(p.age - uTimeStep, 0.0f, p.start_age);
}

void updateParticle(thread TParticle& p,
                    float3 pos,
                    float3 vel,
                    float age) {
    p.position.xyz = pos;
    p.velocity.xyz = vel;
    p.age = age;
}

float3 calculateScattering(float uScatteringFactor,
                           device const float* randbuffer,
                           uint gid) {
    float3 randforce = float3(randbuffer[gid], randbuffer[gid+1u], randbuffer[gid+2u]);
    randforce = 2.0f * randforce - 1.0f;
    return uScatteringFactor * randforce;
}

float3 calculateRepulsion(TParticle p) {
    float3 push = float3(0.0f);
    
    //    // IDEA
    //    const float3 vel = p.velocity.xyz;
    //    const float3 pos = p.position.xyz;
    //    const float MAX_INFLUENCE_DISTANCE = 8.0f;
    //
    //    float3 n;
    //    float d = compute_gradient(pos, n);
    //    float coeff = smoothstep(0.0f, MAX_INFLUENCE_DISTANCE, abs(d));
    //    push = coeff * (n);
    //    //vec3 side = cross(cross(n, normalize(vel + vec3(1e-5))), n);
    //    //push = mix(push, side, coeff*coeff);
    
    return push;
}

float3 calculateTargetMesh(TParticle p) {
    float3 pull = float3(0.0f);
    
    
    //    // IDEA
    //    float3 anchor = anchors_buffer[p.anchor_id];
    //    matrix_float4x4 anchorModel = achor_models_buffer[p.anchor_id];
    //    float3 target = anchorModel * float4(anchor, 1.0f);
    //    float3 pull = target - p.position;
    //    float length_pull = length(pull);
    //    pull *= rsqrt(length_pull);
    //    float factor = MAX_PULL_FACTOR * smoothstep(0.0f, MAX_PULL_FACTOR, length_pull);
    //    pull *= factor;
    
    return pull;
}

float3 calculateVectorField(TParticle p,
                            float uVectorFieldFactor,
                            texture3d<float, access::sample> uVectorFieldTexture,
                            sampler uVectorFieldSampler) {
    const float3 extent = 0.5f * float3(uVectorFieldTexture.get_width(),
                                        uVectorFieldTexture.get_height(),
                                        uVectorFieldTexture.get_depth());
    const float3 texcoord = (p.position.xyz + extent) / (2.0f * extent);
    float3 vfield = uVectorFieldTexture.sample(uVectorFieldSampler, texcoord).xyz;
    
    return uVectorFieldFactor * vfield;
}

float3 calculateCurlNoise(TParticle p,
                          float uCurlNoiseScale,
                          float uCurlNoiseFactor,
                          int uPerlinNoisePermutationSeed) {
    float3 curl_velocity = compute_curl(p.position.xyz * uCurlNoiseScale, uPerlinNoisePermutationSeed);
    return uCurlNoiseFactor * curl_velocity;
}

void collideSphere(float r, float3 center, thread float3& pos, thread float3& vel) {
    const float3 p = pos - center;
    
    const float dp = dot(p, p);
    const float r2 = r*r;
    
    if (dp > r2) {
        float3 n = -p * rsqrt(dp);
        vel = reflect(vel, n);
        
        pos = center - r*n;
    }
}

void collideBox(float3 corner, float3 center, thread float3& pos, thread float3& vel) {
    float3 p = pos - center;
    
    if (p.x < -corner.x) {
        p.x = -corner.x;
        vel = reflect(vel, float3(1.0f,0.0f,0.0f));
    }
    
    if (p.x > corner.x) {
        p.x = corner.x;
        vel = reflect(vel, float3(-1.0f,0.0f,0.0f));
    }
    
    if (p.y < -corner.y) {
        p.y = -corner.y;
        vel = reflect(vel, float3(0.0f,1.0f,0.0f));
    }
    
    if (p.y > corner.y) {
        p.y = corner.y;
        vel = reflect(vel, float3(0.0f,-1.0f,0.0f));
    }
    
    if (p.z < -corner.z) {
        p.z = -corner.z;
        vel = reflect(vel, float3(0.0f,0.0f,1.0f));
    }
    
    if (p.z > corner.z) {
        p.z = corner.z;
        vel = reflect(vel, float3(0.0f,0.0f,-1.0f));
    }
    
    pos = p + center;
}

void collisionHandling(thread float3& pos, thread float3& vel,
                       int uBoundingVolume,
                       float uBBoxSize) {
    const float r = 0.5f * uBBoxSize;
    
    if (uBoundingVolume == 0)
        collideSphere(r, float3(0.0f), pos, vel);
    else
        if (uBoundingVolume == 1)
            collideBox(float3(r), float3(0.0f), pos, vel);
}

kernel void particle_simulation(constant float& uTimeStep [[buffer(0)]],
                                constant int& uBoundingVolume [[buffer(1)]],
                                constant float& uBBoxSize [[buffer(2)]],
                                
                                constant float& uScatteringFactor [[buffer(3), function_constant(needParticleScattering)]],
                                device float* randbuffer [[buffer(4), function_constant(needParticleScattering)]],
                                
                                constant float& uVectorFieldFactor [[buffer(5), function_constant(needParticleVectorField)]],
                                sampler uVectorFieldSampler [[sampler(0), function_constant(needParticleVectorField)]],
                                texture3d<float, access::sample> uVectorFieldTexture [[texture(0), function_constant(needParticleVectorField)]],
                                                                
                                constant float& uCurlNoiseFactor [[buffer(6), function_constant(needParticleCurlNoise)]],
                                constant float& uCurlNoiseScale [[buffer(7), function_constant(needParticleCurlNoise)]],
                                
                                constant float& uVelocityFactor [[buffer(8), function_constant(needParticleVelocityControl)]],

                                device atomic_uint* read_count [[buffer(9)]],
                                device atomic_uint* write_count [[buffer(10)]],
#if USE_SOA_LAYOUT
                                device float4* read_positions [[buffer(11)]],
                                device float4* read_velocities [[buffer(12)]],
                                device float4* read_attributes [[buffer(13)]],
                                device float4* write_positions [[buffer(14)]],
                                device float4* write_velocities [[buffer(15)]],
                                device float4* write_attributes [[buffer(16)]],
#else
                                device TParticle* read_particles [[buffer(17)]],
                                device TParticle* write_particles [[buffer(18)]],
#endif
                                constant int& uPerlinNoisePermutationSeed [[buffer(19)]],
                                uint gid [[ thread_position_in_grid ]]) {
    // Local copy of the particle.
    TParticle p = popParticle(read_count,
#if USE_SOA_LAYOUT
                              read_positions,
                              read_velocities,
                              read_attributes,
#else
                              read_particles,
#endif
                              gid);
    
    float age = updatedAge(p, uTimeStep);
    
    if (age > 0.0f) {
        // Calculate external forces.
        float3 force = float3(0.0f);
        
        if (needParticleScattering) {
            force += calculateScattering(uScatteringFactor, randbuffer, gid);
        }
        force += calculateRepulsion(p);
        force += calculateTargetMesh(p);
        if (needParticleVectorField) {
            force += calculateVectorField(p, uVectorFieldFactor,
                                          uVectorFieldTexture, uVectorFieldSampler);
        }
        if (needParticleCurlNoise) {
            force += calculateCurlNoise(p, uCurlNoiseScale,
                                        uCurlNoiseFactor, uPerlinNoisePermutationSeed);
        }
        
        // Integrations vectors.
        const float3 dt = float3(uTimeStep);
        float3 velocity = p.velocity.xyz;
        float3 position = p.position.xyz;
        
        // Integrate velocity.
        velocity = fma(force, dt, velocity);
        
        if (needParticleVelocityControl) {
            velocity = uVelocityFactor * normalize(velocity);
        }
        
        // Integrate position.
        position = fma(velocity, dt, position);
        
        // Handle collisions.
        collisionHandling(position, velocity,
                          uBoundingVolume,
                          uBBoxSize);
        
        // Update the particle.
        updateParticle(p, position, velocity, age);
        
        // Save it in buffer.
        pushParticle(p, write_count
#if USE_SOA_LAYOUT
                     , write_positions
                     , write_velocities
                     , write_attributes
#else
                     , write_particles
#endif
                     );
    }
}
