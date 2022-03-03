//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_perlin_2d_h
#define particle_perlin_2d_h

#include "particle_perlin.h"

// Classical Perlin Noise 2D
float pnoise(float2 pt, float2 scaledTileRes, int uPerlinNoisePermutationSeed);

float pnoise(float2 pt, int uPerlinNoisePermutationSeed);

// Derivative Perlin Noise 2D
float3 dpnoise(float2 pt, float2 scaledTileRes, int uPerlinNoisePermutationSeed);

float3 dpnoise(float2 pt, int uPerlinNoisePermutationSeed);

/// @note
/// Tiling does not works with non integer frequency or non power of two zoom.
// Classical Perlin Noise fbm 2D
float fbm_pnoise(float2 pt,
                 const float zoom,
                 const int numOctave,
                 const float frequency,
                 const float amplitude,
                 int uPerlinNoisePermutationSeed);

// Derivative Perlin Noise fbm 2D
float fbm_dpnoise(float2 pt,
                  const float zoom,
                  const int numOctave,
                  const float frequency,
                  const float amplitude,
                  int uPerlinNoisePermutationSeed);

#endif /* particle_perlin_2d_h */
