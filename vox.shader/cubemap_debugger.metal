//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "function_constant.h"

typedef struct {
    float3 position [[attribute(Position)]];
    float2 TEXCOORD_0 [[attribute(UV_0)]];
} VertexIn;

typedef struct {
    float4 position [[position]];
    float2 v_uv;
} VertexOut;

vertex VertexOut vertex_cubemap(const VertexIn in [[stage_in]],
                                constant matrix_float4x4 &u_MVPMat [[buffer(7)]]) {
    VertexOut out;
    
    // begin position
    float4 position = float4( in.position, 1.0);
    out.position = u_MVPMat * position;
    out.v_uv = in.TEXCOORD_0;
    return out;
}

fragment float4 fragment_cubemap(VertexOut in [[stage_in]],
                                 texture2d<float> u_baseTexture [[texture(0)]],
                                 constant int& u_faceIndex [[buffer(2)]]) {
    constexpr sampler s(coord::normalized, filter::linear,
                        address::clamp_to_edge, compare_func:: less);
    
    float2 uv = in.v_uv;
    if (u_faceIndex == 2 || u_faceIndex == 3) {
        uv.y = 1 - uv.y;
    }
    
    float4 baseColor = u_baseTexture.sample(s, uv);
    
    return float4(baseColor.x, baseColor.y, baseColor.z, 1);
}
