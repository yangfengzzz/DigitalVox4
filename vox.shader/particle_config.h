//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_config_h
#define particle_config_h

#import <simd/simd.h>

// Kernel group width used across the particles pipeline.
#define PARTICLES_KERNEL_GROUP_WIDTH        512u

// Decide which structure layout to use.
#ifndef SPARKLE_USE_SOA_LAYOUT
#define SPARKLE_USE_SOA_LAYOUT              FALSE
#endif

struct TParticle {
    float4 position;
    float4 velocity;
    float start_age;
    float age;
    float _padding0;
    uint id;
};


#endif /* particle_config_h */
