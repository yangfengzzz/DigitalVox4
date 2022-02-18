//
//  debug_frame.metal
//  vox.shader
//
//  Created by 杨丰 on 2022/2/18.
//

#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;

typedef struct {
    float3 position [[attribute(0)]];
    uint color   [[attribute(1)]];
} VertexIn;

typedef struct {
    float4 position [[position]];
    uint color;
} VertexOut;

vertex VertexOut vertex_debug_frame(const VertexIn in [[stage_in]],
                                    constant matrix_float4x4 &u_MVPMat [[buffer(7)]]) {
    VertexOut out;
    
    out.color = in.color;
    out.position = u_MVPMat * float4( in.position, 1.0 );
    return out;
}

fragment float4 fragment_debug_frame(VertexOut in [[stage_in]]) {
    uint id = in.color;
    return float4(((id & 0xff0000) >> 16) / 255.0, ((id & 0xff00) >> 8) / 255.0, (id & 0xff) / 255.0, 1.0);

}

