//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_perlin_3d_h
#define particle_perlin_3d_h

#include "particle_perlin.h"

// Classical Perlin Noise 3D
float pnoise(float3 pt, float3 scaledTileRes, int uPerlinNoisePermutationSeed);

float pnoise(float3 pt, int uPerlinNoisePermutationSeed);

// Classical Perlin Noise 2D + time
float pnoise_loop(float2 u, float dt, int uPerlinNoisePermutationSeed);

// Classical Perlin Noise fbm 3D
float fbm_pnoise(float3 pt,
                 const float zoom,
                 const int numOctave,
                 const float frequency,
                 const float amplitude,
                 int uPerlinNoisePermutationSeed);

// Classical Perlin Noise fbm 3D with predefined parameters
float fbm_3d(float3 ws);

#endif /* particle_perlin_3d_h */
