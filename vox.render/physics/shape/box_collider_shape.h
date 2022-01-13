//
//  box_collider_shape.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/2.
//

#ifndef box_collider_shape_hpp
#define box_collider_shape_hpp

#include "collider_shape.h"

namespace vox {
namespace physics {
class BoxColliderShape : public ColliderShape {
public:
    BoxColliderShape();
    
    Imath::V3f size();
    
    void setSize(const Imath::V3f &value);
    
    void setWorldScale(const Imath::V3f &scale) override;
    
private:
    Imath::V3f _half = Imath::V3f(0.5, 0.5, 0.5);
};

}
}
#endif /* box_collider_shape_hpp */
