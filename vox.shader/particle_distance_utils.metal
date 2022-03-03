//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_distance_utils.h"

//MARK: - CSG operations

float opUnion(float d1, float d2) {
    return min(d1, d2);
}

float opSmoothUnion(float d1, float d2, float k) {
    float r = exp(-k*d1) + exp(-k*d2);
    return -log(r) / k;
}

float opIntersection(float d1, float d2) {
    return max(d1, d2);
}

float opSubstraction(float d1, float d2) {
    return max(d1, -d2);
}

float3 opRepeat(float3 p, float3 c) {
    return fmod(p, c) - 0.5f*c;
}

float opDisplacement(float3 p, float d) {
    p = d*p;
    return sin(p.x)*sin(p.y)*sin(p.z);
}

//MARK: - PRIMITIVEs

float sdPlane(float3 p, float4 n) {
    //n.xyz = normalize(n.xyz);
    return n.w + dot(p, n.xyz);
}

float sdSphere(float3 p, float r) {
    return length(p) - r;
}

float udRoundBox(float3 p, float3 b, float r) {
    return length(max(abs(p)-b, 0.0f)) - r;
}

float sdCylinder(float3 p, float c) {
    return length(p.xy) - c;
}

float sdCylinder(float3 p, float3 c) {
    return opIntersection(length(p.xz-c.xy) - c.z, abs(p.y)-c.y);
}

float sdTorus(float3 p, float2 t) {
    float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}
