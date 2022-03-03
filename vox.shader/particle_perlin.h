//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_perlin_h
#define particle_perlin_h

#include "particle_config.h"

#define NOISE_ENABLE_TILING 0
#define NOISE_TILE_RES float3(512.0f)

// Fast computation of x modulo 289
float3 mod289(float3 x);

float4 mod289(float4 x);

// Compute indices for the PRNG
float4 permute(float4 x, int uPerlinNoisePermutationSeed);

// Quintic interpolant
float2 fade(float2 u);

float3 fade(float3 u);

float normalizeNoise(float n);

#endif /* particle_perlin_h */
