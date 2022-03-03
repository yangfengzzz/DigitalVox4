//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_emission_h
#define particle_emission_h

#include "particle_math.h"

void createParticle(device float* randbuffer,
                    float3 uEmitterPosition,
                    float uEmitterRadius,
                    uint uEmitCount,
                    uint uEmitterType,
                    const uint gid);

#endif /* particle_emission_h */
