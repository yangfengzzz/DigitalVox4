//
//  bounding_frustum.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/25.
//

#include "bounding_frustum.h"
#include "collision_util.h"

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

BoundingFrustum::BoundingFrustum(std::optional<M44f> matrix) {
    near = Plane3f();
    far = Plane3f();
    left = Plane3f();
    right = Plane3f();
    top = Plane3f();
    bottom = Plane3f();
    
    if (matrix) {
        this->calculateFromMatrix(matrix.value());
    }
}

Plane3f BoundingFrustum::getPlane(int index) const {
    switch (index) {
        case 0:
            return near;
        case 1:
            return far;
        case 2:
            return left;
        case 3:
            return right;
        case 4:
            return top;
        case 5:
            return bottom;
        default:
            assert(false && "out of bounds");
    }
}

void BoundingFrustum::calculateFromMatrix(const M44f &matrix) {
    const auto &me = matrix.getValue();
    const auto &m11 = me[0];
    const auto &m12 = me[1];
    const auto &m13 = me[2];
    const auto &m14 = me[3];
    const auto &m21 = me[4];
    const auto &m22 = me[5];
    const auto &m23 = me[6];
    const auto &m24 = me[7];
    const auto &m31 = me[8];
    const auto &m32 = me[9];
    const auto &m33 = me[10];
    const auto &m34 = me[11];
    const auto &m41 = me[12];
    const auto &m42 = me[13];
    const auto &m43 = me[14];
    const auto &m44 = me[15];
    
    // near
    auto &nearNormal = near.normal;
    nearNormal.x = -m14 - m13;
    nearNormal.y = -m24 - m23;
    nearNormal.z = -m34 - m33;
    near.distance = -m44 - m43;
    near = normalize(near);
    
    // far
    auto &farNormal = far.normal;
    farNormal.x = m13 - m14;
    farNormal.y = m23 - m24;
    farNormal.z = m33 - m34;
    far.distance = m43 - m44;
    far = normalize(far);
    
    // left
    auto &leftNormal = left.normal;
    leftNormal.x = -m14 - m11;
    leftNormal.y = -m24 - m21;
    leftNormal.z = -m34 - m31;
    left.distance = -m44 - m41;
    left = normalize(left);
    
    // right
    auto &rightNormal = right.normal;
    rightNormal.x = m11 - m14;
    rightNormal.y = m21 - m24;
    rightNormal.z = m31 - m34;
    right.distance = m41 - m44;
    right = normalize(right);
    
    // top
    auto &topNormal = top.normal;
    topNormal.x = m12 - m14;
    topNormal.y = m22 - m24;
    topNormal.z = m32 - m34;
    top.distance = m42 - m44;
    top = normalize(top);
    
    // bottom
    auto &bottomNormal = bottom.normal;
    bottomNormal.x = -m14 - m12;
    bottomNormal.y = -m24 - m22;
    bottomNormal.z = -m34 - m32;
    bottom.distance = -m44 - m42;
    bottom = normalize(bottom);
}

bool BoundingFrustum::intersectsBox(const BoundingBox3f &box) const {
    return intersectsFrustumAndBox(*this, box);
}

bool BoundingFrustum::intersectsSphere(const Sphere3f &sphere) const {
    return frustumContainsSphere(*this, sphere) != ContainmentType::Disjoint;
}

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT
