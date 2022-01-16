//
//  bounding_frustum.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/25.
//

#include "bounding_frustum.h"
#include "collision_util.h"

namespace vox {
BoundingFrustum::BoundingFrustum() {
}

BoundingFrustum::BoundingFrustum(const Matrix4x4F& matrix) {
    near = Plane3F();
    far = Plane3F();
    left = Plane3F();
    right = Plane3F();
    top = Plane3F();
    bottom = Plane3F();
    calculateFromMatrix(matrix);
}

Plane3F BoundingFrustum::getPlane(int index) const {
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

void BoundingFrustum::calculateFromMatrix(const Matrix4x4F& matrix) {
    const auto &m11 = matrix[0];
    const auto &m12 = matrix[1];
    const auto &m13 = matrix[2];
    const auto &m14 = matrix[3];
    const auto &m21 = matrix[4];
    const auto &m22 = matrix[5];
    const auto &m23 = matrix[6];
    const auto &m24 = matrix[7];
    const auto &m31 = matrix[8];
    const auto &m32 = matrix[9];
    const auto &m33 = matrix[10];
    const auto &m34 = matrix[11];
    const auto &m41 = matrix[12];
    const auto &m42 = matrix[13];
    const auto &m43 = matrix[14];
    const auto &m44 = matrix[15];
    
    // near
    auto &nearNormal = near.normal;
    nearNormal.x = -m14 - m13;
    nearNormal.y = -m24 - m23;
    nearNormal.z = -m34 - m33;
    near.distance = -m44 - m43;
    near.normalize();
    
    // far
    auto &farNormal = far.normal;
    farNormal.x = m13 - m14;
    farNormal.y = m23 - m24;
    farNormal.z = m33 - m34;
    far.distance = m43 - m44;
    far.normalize();
    
    // left
    auto &leftNormal = left.normal;
    leftNormal.x = -m14 - m11;
    leftNormal.y = -m24 - m21;
    leftNormal.z = -m34 - m31;
    left.distance = -m44 - m41;
    left.normalize();
    
    // right
    auto &rightNormal = right.normal;
    rightNormal.x = m11 - m14;
    rightNormal.y = m21 - m24;
    rightNormal.z = m31 - m34;
    right.distance = m41 - m44;
    right.normalize();
    
    // top
    auto &topNormal = top.normal;
    topNormal.x = m12 - m14;
    topNormal.y = m22 - m24;
    topNormal.z = m32 - m34;
    top.distance = m42 - m44;
    top.normalize();
    
    // bottom
    auto &bottomNormal = bottom.normal;
    bottomNormal.x = -m14 - m12;
    bottomNormal.y = -m24 - m22;
    bottomNormal.z = -m34 - m32;
    bottom.distance = -m44 - m42;
    bottom.normalize();
}

bool BoundingFrustum::intersectsBox(const BoundingBox3F &box) const {
    return intersectsFrustumAndBox(*this, box);
}

}
