//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_math_h
#define particle_math_h

#include "particle_config.h"

float pi();

float twoPi();

float goldenAngle();

float cbrt(float x);

matrix_float3x3 rotationX(float c, float s);

matrix_float3x3 rotationY(float c, float s);

matrix_float3x3 rotationZ(float c, float s);

matrix_float3x3 rotationX(float radians);

matrix_float3x3 rotationY(float radians);

matrix_float3x3 rotationZ(float radians);

float3 disk_distribution(float radius, float2 rn);

float3 disk_even_distribution(float radius, uint id, uint total);

float3 sphere_distribution(float radius, float2 rn);

float3 ball_distribution(float radius, float3 rn);

#endif /* particle_math_h */
