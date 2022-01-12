//
//  hit_result.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#ifndef hit_result_hpp
#define hit_result_hpp

#include "ImathVec.h"
#include "../entity.h"

namespace vox {
namespace physics {

/**
 * Structure used to get information back from a raycast or a sweep.
 */
struct HitResult {
    /** The entity that was hit. */
    sg::Entity *entity = nullptr;
    /** The distance from the ray's origin to the impact point. */
    float distance = 0;
    /** The impact point in world space where the ray hit the collider. */
    Imath::V3f point = Imath::V3f();
    /** The normal of the surface the ray hit. */
    Imath::V3f normal = Imath::V3f();
};

}
}


#endif /* hit_result_hpp */
