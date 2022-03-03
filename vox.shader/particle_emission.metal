//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_emission.h"

void createParticle(device float* randbuffer,
                    float3 uEmitterPosition,
                    float uEmitterRadius,
                    uint uEmitCount,
                    uint uEmitterType,
                    const uint gid) {
    // Random vector.
    const uint rid = 3u * gid;
    const float3 rn = float3(randbuffer[rid], randbuffer[rid+1u], randbuffer[rid+2u]);
    
    // Position
    float3 pos = uEmitterPosition;
    if (uEmitterType == 1) {
        //pos += disk_distribution(uEmitterRadius, rn.xy);
        pos += disk_even_distribution(uEmitterRadius, gid, uEmitCount);
    } else if (uEmitterType == 2) {
        pos += sphere_distribution(uEmitterRadius, rn.xy);
    } else if (uEmitterType == 3) {
        pos += ball_distribution(uEmitterRadius, rn);
    }
}


