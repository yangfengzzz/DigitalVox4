//
//  plane_collider_shape.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#include "plane_collider_shape.h"
#include "../physics_manager.h"

namespace vox {
namespace physics {
PlaneColliderShape::PlaneColliderShape() {
    _nativeGeometry = std::make_shared<PxPlaneGeometry>();
    _nativeShape = PhysicsManager::_nativePhysics()->createShape(*_nativeGeometry, *_nativeMaterial, true);
    _nativeShape->setQueryFilterData(PxFilterData(PhysicsManager::_idGenerator++, 0, 0, 0));
    setLocalPose(_pose);
}

Imath::Eulerf PlaneColliderShape::rotation() {
    Imath::Eulerf euler;
    euler.extract(_pose.orientation());
    return euler;
}

void PlaneColliderShape::setRotation(const Imath::V3f &value) {
    auto rotation = Imath::Eulerf(value.x, value.y, value.z).toQuat();
    Imath::Quatf rotateZ;
    rotateZ.setAxisAngle(Imath::V3f(0, 0, 1), M_PI * 0.5);
    rotation *= rotateZ;
    rotation.normalize();
    _pose.setOrientation(rotation);
    setLocalPose(_pose);
}

}
}
