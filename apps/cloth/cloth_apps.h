//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cloth_apps_h
#define cloth_apps_h

#include "sphere_app.h" // sphere collider
#include "capsule_app.h" // capsule collider
#include "ccd_app.h" // capsule collider with animation ccd
#include "plane_collision_app.h" // plane collider
#include "free_fall_app.h" // plane collider
#include "friction_app.h" // friction comparsion
#include "convex_collision_app.h" // convex collider
#include "triangle_app.h" // mesh collider
#include "distance_constraint_app.h" // distance constraints
#include "virtual_particle_app.h" // virtual particles

#include "stiffness_per_constraint_app.h" // setTetherConstraintStiffness
#include "geodesic_app.h" // geodesic distance for tether constraints

#include "inter_collision_app.h" // lap together
#include "self_collision_app.h" // self lap together

#include "multi_solver_app.h"
#include "local_global_app.h"

#include "teleport_app.h"
#include "tether_app.h"
#include "time_step_app.h"
#include "wind_app.h"

#endif /* cloth_apps_h */
