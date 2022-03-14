//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
#include "particle_draw_common.h"
using namespace metal;

/* Map a range from [edge0, edge1] to [0, 1]. */
float maprange(float edge0, float edge1, float x) {
    return clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
}

/* Map a value in [0, 1] to peak at edge. */
float curve_inout(float x, float edge) {
    // Coefficient for sub range.
    float a = maprange(0.0f, edge, x);
    float b = maprange(edge, 1.0f, x);
    
    // Quadratic ease-in / quadratic ease-out.
    float easein = a * (2.0f - a);        // a * a;
    float easeout = b*b - 2.0f*b + 1.0f;  // 1.0f - b * b;
    
    // chose between easin / easout function.
    float result = mix(easein, easeout, step(edge, x));
    
    // Makes particles fade-in and out of existence
    return result;
}

float compute_size(float z, float decay,
                   float uMinParticleSize,
                   float uMaxParticleSize) {
    const float min_size = uMinParticleSize;
    const float max_size = uMaxParticleSize;
    
    // tricks to 'zoom-in' the pointsprite, just set to 1 to have normal size.
    const float depth = (max_size-min_size) / (z);
    
    float size = mix(min_size, max_size, decay * depth);
    
    return size;
}

float3 base_color(float3 position, float decay,
                  float uColorMode,
                  float3 uBirthGradient,
                  float3 uDeathGradient) {
    // Gradient mode
    if (uColorMode == 1) {
        return mix(uBirthGradient, uDeathGradient, decay);
    }
    // Default mode
    return 0.5f * (normalize(position) + 1.0f);
}

float4 compute_color(float3 base_color, float decay, float2 texcoord,
                     float uFadeCoefficient, bool uDebugDraw) {
    if (uDebugDraw) {
        return float4(1.0f);
    }
    
    float4 color = float4(base_color, 1.0f);
    
    // Centered coordinates.
    const float2 p = 2.0f * (texcoord - 0.5f);
    // Pixel intensity depends on its distance from center.
    float d = 1.0f - abs(dot(p, p));
    
    // Alpha coefficient.
    float alpha = smoothstep(0.0f, 1.0f, d);
    
    //color = texture(uSpriteSampler2d, texcoord).rrrr;
    color *= alpha * decay * uFadeCoefficient;
    
    return color;
}
