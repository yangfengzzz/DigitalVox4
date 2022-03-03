//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_math.h"

float pi() {
    return 3.141564f;
}

float twoPi() {
    return 6.283185f;
}

float goldenAngle() {
    return 2.399963f;
}

float cbrt(float x) {
    return pow(x, 0.33333f);
}

matrix_float3x3 rotationX(float c, float s) {
    return matrix_float3x3(float3(1.0f, 0.0f, 0.0f),
                           float3(0.0f, c, s),
                           float3(0.0f, -s, c));
}

matrix_float3x3 rotationY(float c, float s) {
    return matrix_float3x3(float3(c, 0.0f, -s),
                           float3(0.0f, 1.0f, 0.0f),
                           float3(s, 0.0f, c));
}

matrix_float3x3 rotationZ(float c, float s) {
    return matrix_float3x3(float3(c, s, 0.0f),
                           float3(-s, c, 0.0f),
                           float3(0.0f, 0.0f, 1.0f));
}

matrix_float3x3 rotationX(float radians) {
    return rotationX(cos(radians), sin(radians));
}

matrix_float3x3 rotationY(float radians) {
    return rotationY(cos(radians), sin(radians));
}

matrix_float3x3 rotationZ(float radians) {
    return rotationZ(cos(radians), sin(radians));
}

float3 disk_distribution(float radius, float2 rn) {
    const float r = radius * rn.x;
    const float theta = twoPi() * rn.y;
    return float3(r * cos(theta),
                  0.0f,
                  r * sin(theta));
}

float3 disk_even_distribution(float radius, uint id, uint total) {
    // ref : http://blog.marmakoide.org/?p=1
    const float theta = id * goldenAngle();
    const float r = radius *  sqrt(id / float(total));
    return float3(r * cos(theta),
                  0.0f,
                  r * sin(theta));
}

float3 sphere_distribution(float radius, float2 rn) {
    // ref : https://www.cs.cmu.edu/~mws/rpos.html
    //       https://gist.github.com/dinob0t/9597525
    const float phi = twoPi() * rn.x;
    const float z = radius * (2.0f * rn.y - 1.0f);
    const float r = sqrt(radius * radius - z * z);
    return float3(r * cos(phi),
                  r * sin(phi),
                  z);
}

float3 ball_distribution(float radius, float3 rn) {
    // ref : so@5408276
    const float costheta = 2.0f * rn.x - 1.0f;
    const float phi = twoPi() * rn.y;
    const float theta = acos(costheta);
    const float r = radius * cbrt(rn.z);
    const float s = sin(theta);
    
    return r * float3(s * cos(phi),
                      s * sin(phi),
                      costheta);
}
