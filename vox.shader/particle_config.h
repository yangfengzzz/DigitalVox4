//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_config_h
#define particle_config_h

#import <simd/simd.h>

struct TParticle {
    simd_float4 position;
    simd_float4 velocity;
    float start_age;
    float age;
    float _padding0;
    uint32_t id;
};


#endif /* particle_config_h */
