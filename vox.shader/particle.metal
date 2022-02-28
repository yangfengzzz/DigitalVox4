//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;

typedef struct {
    float3 a_position [[attribute(0)]];
    float4 a_color [[attribute(1)]];
} VertexIn;

typedef struct {
    float4 position [[position]];
    float4 v_color;
    float pointSize [[point_size]];
} VertexOut;

vertex VertexOut vertex_particle(const VertexIn vertexIn [[stage_in]],
                                 constant matrix_float4x4 &u_VPMat [[buffer(12)]]) {
    VertexOut out;

    out.position = u_VPMat * float4(vertexIn.a_position, 1.0);
    out.pointSize = 15;
    out.v_color = vertexIn.a_color;
    
    return out;
}

fragment float4 fragment_particle(VertexOut in [[stage_in]],
                                  texture2d<float> u_particleTexture [[texture(0)]],
                                  float2 point [[point_coord]]) {    
    if (length(point - float2(0.5)) > 0.5) {
        discard_fragment();
    }
    
    constexpr sampler default_sampler;
    float4 color = u_particleTexture.sample(default_sampler, point);
    color *= in.v_color;
    color.a = 0.4;
    return color;
}

