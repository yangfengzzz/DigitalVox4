//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_draw_common_h
#define particle_draw_common_h

#include <simd/simd.h>
using namespace metal;

/* Map a range from [edge0, edge1] to [0, 1]. */
float maprange(float edge0, float edge1, float x);

/* Map a value in [0, 1] to peak at edge. */
float curve_inout(float x, float edge);

float compute_size(float z, float decay,
                   float uMinParticleSize,
                   float uMaxParticleSize);

float3 base_color(float3 position, float decay,
                  float uColorMode,
                  float3 uBirthGradient,
                  float3 uDeathGradient);

float4 compute_color(float3 base_color, float decay, float2 texcoord,
                     float uFadeCoefficient, bool uDebugDraw);

#endif /* particle_draw_common_h */
