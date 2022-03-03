//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_distance_utils_h
#define particle_distance_utils_h

#include "particle_config.h"

//MARK: - CSG operations

float opUnion(float d1, float d2);

float opSmoothUnion(float d1, float d2, float k);

float opIntersection(float d1, float d2);

float opSubstraction(float d1, float d2);

float3 opRepeat(float3 p, float3 c);

float opDisplacement(float3 p, float d);

//MARK: - PRIMITIVEs

float sdPlane(float3 p, float4 n);

float sdSphere(float3 p, float r);

float udRoundBox(float3 p, float3 b, float r);

float sdCylinder(float3 p, float c);

float sdCylinder(float3 p, float3 c);

float sdTorus(float3 p, float2 t);

#endif /* particle_distance_utils_h */
