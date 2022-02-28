//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "function-constant.h"

typedef struct {
    float3 position [[attribute(Position)]];
    float2 TEXCOORD_0 [[attribute(UV_0)]];
} VertexIn;

typedef struct {
    float4 position [[position]];
    float2 v_uv;
} VertexOut;

vertex VertexOut vertex_background_texture(const VertexIn vertexIn [[stage_in]]) {
    VertexOut out;
    out.position = float4(vertexIn.position, 1.0);
    out.v_uv = vertexIn.TEXCOORD_0;
    return out;
}

fragment float4 fragment_background_texture(VertexOut in [[stage_in]],
                                            texture2d<float> u_baseTexture [[texture(0)]]) {
    constexpr sampler textureSampler(coord::normalized, filter::linear,
                                     address::repeat, compare_func:: less);
    
    return u_baseTexture.sample(textureSampler, in.v_uv);
}
