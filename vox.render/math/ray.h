//
//  ray.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/10.
//

#ifndef ray_hpp
#define ray_hpp

#include "common/glm_common.h"

namespace vox {
/**
 * Represents a ray with an origin and a direction in 3D space.
 */
class Ray {
public:
    /** The origin of the ray. */
    glm::vec3 origin;
    /** The normalized direction of the ray. */
    glm::vec3 direction;
};

}

#endif /* ray_hpp */
