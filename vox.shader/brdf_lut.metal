//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "pbr_common.h"

kernel void integrateBRDF(texture2d<float, access::write> lut [[ texture(0) ]],
                          uint2 position [[ thread_position_in_grid ]]) {
    
    float width = lut.get_width();
    float height = lut.get_height();
    if (position.x >= width || position.y >= height) {
        return;
    }
    float Roughness = (position.x + 16.0) / width;
    float NoV = (position.y + 1.0) / height;
    
    // input (Roughness and cosTheta) - output (scale and bias to F0)
    float2 brdf = IntegrateBRDF(Roughness, NoV);
    float4 color(brdf, 0, 0);
    lut.write(color, position);
}
