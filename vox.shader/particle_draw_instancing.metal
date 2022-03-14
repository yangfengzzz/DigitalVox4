//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
#include "particle_config.h"
#include "particle_draw_common.h"
using namespace metal;

constant array<float2, 4> pos = {
    float2(-1.0, 1.0),
    float2(-1.0, -1.0),
    float2(1.0, 1.0),
    float2(1.0, -1.0)
};

struct VertexOut {
    float4 position [[position]];
    float2 uv;
    float3 color;
    float decay;
};

vertex VertexOut vertex_particle_instancing(constant float& uMinParticleSize [[buffer(7)]],
                                            constant float& uMaxParticleSize [[buffer(8)]],
                                            constant float& uColorMode [[buffer(9)]],
                                            constant float3& uBirthGradient [[buffer(10)]],
                                            constant float3& uDeathGradient [[buffer(11)]],
                                            const device TParticle *uWriteParticle [[buffer(12)]],
                                            constant matrix_float4x4 &u_viewMat [[buffer(13)]],
                                            constant matrix_float4x4 &u_projMat [[buffer(14)]],
                                            uint instanceIndex [[ instance_id ]],
                                            uint vertexIndex [[ vertex_id ]]) {
    VertexOut out;
    
    // Time alived in [0, 1].
    const float dAge = 1.0f - maprange(0.0f, uWriteParticle[instanceIndex].start_age,
                                       uWriteParticle[instanceIndex].age);
    const float decay = curve_inout(dAge, 0.55f);
    
    out.uv = pos[vertexIndex];
    float3 worldPos = float3(out.uv, 0.0) * compute_size(out.position.z/out.position.w, decay,
                                                         uMinParticleSize, uMaxParticleSize) * 0.025;
    
    // Generate a billboarded model view matrix
    matrix_float4x4 bbModelViewMatrix = matrix_float4x4(1.0);
    bbModelViewMatrix[3] = float4(uWriteParticle[instanceIndex].position.xyz, 1.0);
    bbModelViewMatrix = u_viewMat * bbModelViewMatrix;
    bbModelViewMatrix[0][0] = 1.0;
    bbModelViewMatrix[0][1] = 0.0;
    bbModelViewMatrix[0][2] = 0.0;
    
    bbModelViewMatrix[1][0] = 0.0;
    bbModelViewMatrix[1][1] = 1.0;
    bbModelViewMatrix[1][2] = 0.0;
    
    bbModelViewMatrix[2][0] = 0.0;
    bbModelViewMatrix[2][1] = 0.0;
    bbModelViewMatrix[2][2] = 1.0;
    out.position = u_projMat * bbModelViewMatrix * float4(worldPos, 1.0);
    
    // Output parameters.
    out.color = base_color(uWriteParticle[instanceIndex].position.xyz, decay,
                           uColorMode, uBirthGradient, uDeathGradient);
    out.decay = decay;
    
    return out;
}

fragment float4 fragment_particle_instancing(VertexOut in [[stage_in]],
                                             constant float& uFadeCoefficient [[buffer(5)]],
                                             constant bool& uDebugDraw [[buffer(6)]]) {
    return compute_color(in.color, in.decay, in.uv,
                         uFadeCoefficient, uDebugDraw);
}
