//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_config.h"

float4 positionWS(
#if USE_SOA_LAYOUT
                  device float4* positions,
#else
                  device TParticle* particles,
#endif
                  uint id)
{
    float4 pos;
#if USE_SOA_LAYOUT
    pos = positions[id];
#else
    pos = particles[id].position;
#endif
    return float4(pos.xyz, 1.0f);
}

kernel void particle_calculate_dp(
#if USE_SOA_LAYOUT
                                  device float4* positions [[buffer(0)]],
#else
                                  device TParticle* positions [[buffer(0)]],
#endif
                                  device float* dp  [[buffer(1)]],
                                  constant matrix_float4x4& uViewMatrix [[buffer(2)]],
                                  uint gid [[ thread_position_in_grid ]])
{
    // Transform a particle's position from world space to view space.
    float4 positionVS = uViewMatrix * positionWS(positions, gid);
    
    // The default front of camera in view space.
    const float3 targetVS = float3(0.0f, 0.0f, -1.0f);
    
    // Distance of the particle from the camera.
    dp[gid] = dot(targetVS, positionVS.xyz);
}
