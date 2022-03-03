//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_distance_func_h
#define particle_distance_func_h

#include "particle_config.h"

// Return the distance to the closest object and its normal.
float compute_gradient(float3 p, thread float3& normal);

// Return the distance to the closest object.
float sample_distance(float3 p);

#endif /* particle_distance_func_h */
