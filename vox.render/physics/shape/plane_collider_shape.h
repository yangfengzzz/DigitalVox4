//
//  plane_collider_shape.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#ifndef plane_collider_shape_hpp
#define plane_collider_shape_hpp

#include "collider_shape.h"

namespace vox {
namespace physics {
/**
 * Physical collider shape plane.
 */
class PlaneColliderShape : public ColliderShape {
public:
    PlaneColliderShape();
    
    /**
     * The local rotation of this plane.
     */
    Imath::Eulerf rotation();
    
    void setRotation(const Imath::V3f &value);
    
    void setWorldScale(const Imath::V3f &scale) {
    }
};

}
}
#endif /* plane_collider_shape_hpp */
