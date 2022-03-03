//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_curl_noise_h
#define particle_curl_noise_h

#include "particle_perlin_3d.h"
#include "particle_distance_func.h"

float3 compute_curl(float3 p, int uPerlinNoisePermutationSeed);

float3 sample_potential(float3 p, int uPerlinNoisePermutationSeed);

void match_boundary(float inv_noise_scale, float d, float3 normal, thread float3& psi);

// higher order smoothstep.
float smoothstep_2(float edge0, float edge1, float x);

// smoothed a value in [-1, 1]
float ramp(float x);

// return a vector of noise values.
float3 noise3d(float3 seed, int uPerlinNoisePermutationSeed);

#endif /* particle_curl_noise_h */
