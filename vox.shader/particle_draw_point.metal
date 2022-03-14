//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
#include "particle_draw_common.h"
using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 velocity [[attribute(1)]];
    float2 age_info [[attribute(2)]];
};

struct VertexOut {
    float4 position [[position]];
    float pointSize [[point_size]];
    float3 color;
    float decay;
};

vertex VertexOut vertex_particle_point(const VertexIn in [[stage_in]],
                                       constant matrix_float4x4& u_MVPMat [[buffer(6)]],
                                       constant float& uMinParticleSize [[buffer(7)]],
                                       constant float& uMaxParticleSize [[buffer(8)]],
                                       constant float& uColorMode [[buffer(9)]],
                                       constant float3& uBirthGradient [[buffer(10)]],
                                       constant float3& uDeathGradient [[buffer(11)]]) {
    VertexOut out;
    
    // Time alived in [0, 1].
    const float dAge = 1.0f - maprange(0.0f, in.age_info.x, in.age_info.y);
    const float decay = curve_inout(dAge, 0.55f);
    
    // Vertex attributes.
    out.position = u_MVPMat * float4(in.position.xyz, 1.0f);
    out.pointSize = compute_size(out.position.z/out.position.w, decay, uMinParticleSize, uMaxParticleSize);
    
    // Output parameters.
    out.color = base_color(in.position, decay, uColorMode, uBirthGradient, uDeathGradient);
    out.decay = decay;
    
    return out;
}

fragment float4 fragment_particle_point(VertexOut in [[stage_in]],
                                        constant float& uFadeCoefficient [[buffer(5)]],
                                        constant bool& uDebugDraw [[buffer(6)]],
                                        float2 point [[point_coord]]) {
    return compute_color(in.color, in.decay, point,
                         uFadeCoefficient, uDebugDraw);
}
