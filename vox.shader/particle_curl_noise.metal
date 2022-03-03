//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_curl_noise.h"

float3 compute_curl(float3 p, int uPerlinNoisePermutationSeed) {
    const float eps = 1e-4f;
    
    const float3 dx = float3(eps, 0.0f, 0.0f);
    const float3 dy = dx.yxy;
    const float3 dz = dx.yyx;
    
    float3 p00 = sample_potential(p + dx, uPerlinNoisePermutationSeed);
    float3 p01 = sample_potential(p - dx, uPerlinNoisePermutationSeed);
    float3 p10 = sample_potential(p + dy, uPerlinNoisePermutationSeed);
    float3 p11 = sample_potential(p - dy, uPerlinNoisePermutationSeed);
    float3 p20 = sample_potential(p + dz, uPerlinNoisePermutationSeed);
    float3 p21 = sample_potential(p - dz, uPerlinNoisePermutationSeed);
    
    float3 v;
    v.x = p11.z - p10.z - p21.y + p20.y;
    v.y = p21.x - p20.x - p01.z + p00.z;
    v.z = p01.y - p00.y - p11.x + p10.x;
    v /= (2.0f*eps);
    
    return v;
}

// [ User customized sampling function ]
float3 sample_potential(float3 p, int uPerlinNoisePermutationSeed) {
    const uint num_octaves = 4u;
    
    // Potential
    float3 psi = float3(0.0f);
    
    // Compute normal and retrieve distance from colliders.
    float3 normal;
    float distance = compute_gradient(p, normal);
    
    /*
     // --------
     const float PlumeCeiling = (0);
     const float PlumeBase = (-3.0);
     const float PlumeHeight = (80);
     const float RingRadius = (10.25f);
     const float RingSpeed = (0.3f);
     const float RingsPerSecond = (0.125f);
     const float RingMagnitude = (10);
     const float RingFalloff = (0.7f);
     //---------
     */
    
    float height_factor = 1.0;//ramp((p.y - PlumeBase)/ PlumeHeight);
    
    // Add turbulence octaves that respects boundaries.
    float noise_gain = 1.0f;
    for(uint i=0u; i < num_octaves; ++i, noise_gain *= 0.5f) {
        // const float noise_scale = 0.42f * noise_gain;
        const float inv_noise_scale = 1.0f / noise_gain;
        
        float3 s = p * inv_noise_scale;
        float3 n = noise3d(s, uPerlinNoisePermutationSeed);
        
        match_boundary(inv_noise_scale, distance, normal, psi);
        psi += height_factor * noise_gain * n;
    }
    
    // [ add custom potentials ]
    
    // --------
    
    /*
     vec3 rising_force = vec3(-p.z, 0.0f, p.x);
     
     float ring_y = PlumeCeiling;
     float d = ramp(abs(distance) / RingRadius);
     
     while (ring_y > PlumeBase) {
     float ry = p.y - ring_y;
     float rr = sqrt(dot(p.xz, p.xz));
     vec3 v = vec3(rr-RingRadius, rr+RingRadius, ry);
     float rmag = RingMagnitude / (dot(v,v) + RingFalloff);
     vec3 rpsi = rmag * rising_force;
     psi += mix(dot(rpsi, normal)*normal, psi, d);
     ring_y -= RingSpeed / RingsPerSecond;
     }
     */
    
    return psi;
}

void match_boundary(float inv_noise_scale, float d, float3 normal, thread float3& psi) {
    float alpha = ramp(abs(d) * inv_noise_scale);
    float dp = dot(psi, normal);
    psi = mix(dp * normal, psi, alpha);
}

// ----------------------------------------------------------------------------

float smoothstep_2(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (10.0f + t *(-15.0f + 6.0f * t));
}

float ramp(float x) {
    return smoothstep_2(-1.0f, 1.0f, x) * 2.0f - 1.0f;
}

float3 noise3d(float3 seed, int uPerlinNoisePermutationSeed) {
    return float3(pnoise(seed, uPerlinNoisePermutationSeed),
                  pnoise(seed + float3(31.416f, -47.853f, 12.793f), uPerlinNoisePermutationSeed),
                  pnoise(seed + float3(-233.145f, -113.408f, -185.31f), uPerlinNoisePermutationSeed));
}
