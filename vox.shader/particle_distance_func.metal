//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_distance_func.h"

float compute_gradient(float3 p, float3 normal) {
    const float d = sample_distance(p);
    
    const float2 eps = float2(1e-2f, 0.0f);
    normal.x = sample_distance(p + eps.xyy) - d;
    normal.y = sample_distance(p + eps.yxy) - d;
    normal.z = sample_distance(p + eps.yyx) - d;
    normal = normalize(normal);
    
    return d;
}

// [USER DEFINED]
float sample_distance(float3 p) {
    return p.y; //sdSphere(p - vec3(30.0f, 110.0f, 0.0f), 64.0);
}
