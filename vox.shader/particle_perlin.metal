//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_perlin.h"

// Fast computation of x modulo 289
float3 mod289(float3 x) {
    return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}

float4 mod289(float4 x) {
    return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}

// Compute indices for the PRNG
float4 permute(float4 x,
               int uPerlinNoisePermutationSeed) {
    return mod289(((x*34.0f)+1.0f)*x + uPerlinNoisePermutationSeed);
}

// Quintic interpolant
float2 fade(float2 u) {
    return u*u*u*(u*(u*6.0f - 15.0f) + 10.0f);
    
    // Original cubic interpolant (faster, but not 2nd order derivable)
    //return u*u*(3.0f - 2.0f*u);
}

float3 fade(float3 u) {
    return u*u*u*(u*(u*6.0f - 15.0f) + 10.0f);
}

float normalizeNoise(float n) {
    // return noise in [0, 1]
    return 0.5f*(2.44f*n + 1.0f);
}
