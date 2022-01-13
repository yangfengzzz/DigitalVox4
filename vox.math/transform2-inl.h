// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef INCLUDE_JET_DETAIL_TRANSFORM2_INL_H_
#define INCLUDE_JET_DETAIL_TRANSFORM2_INL_H_

#include "transform2.h"

#include <algorithm>
#include <cmath>

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

inline Transform2::Transform2() {
}

inline Transform2::Transform2(const V2d& translation,
                              double orientation)
: _translation(translation)
, _orientation(orientation) {
    _cosAngle = std::cos(orientation);
    _sinAngle = std::sin(orientation);
}

inline const V2d& Transform2::translation() const {
    return _translation;
}

inline void Transform2::setTranslation(const V2d& translation) {
    _translation = translation;
}

inline double Transform2::orientation() const {
    return _orientation;
}

inline void Transform2::setOrientation(double orientation) {
    _orientation = orientation;
    _cosAngle = std::cos(orientation);
    _sinAngle = std::sin(orientation);
}

inline V2d Transform2::toLocal(const V2d& pointInWorld) const {
    // Convert to the local frame
    V2d xmt = pointInWorld - _translation;
    return V2d(_cosAngle * xmt.x + _sinAngle * xmt.y,
               -_sinAngle * xmt.x + _cosAngle * xmt.y);
}

inline V2d Transform2::toLocalDirection(const V2d& dirInWorld) const {
    // Convert to the local frame
    return V2d(_cosAngle * dirInWorld.x + _sinAngle * dirInWorld.y,
               -_sinAngle * dirInWorld.x + _cosAngle * dirInWorld.y);
}

inline Ray2d Transform2::toLocal(const Ray2d& rayInWorld) const {
    return Ray2d(toLocal(rayInWorld.origin),
                 toLocalDirection(rayInWorld.direction));
}

inline BoundingBox2d Transform2::toLocal(const BoundingBox2d& bboxInWorld) const {
    BoundingBox2d bboxInLocal;
    for (int i = 0; i < 4; ++i) {
        auto cornerInLocal = toLocal(bboxInWorld.corner(i));
        bboxInLocal.lowerCorner
        = min(bboxInLocal.lowerCorner, cornerInLocal);
        bboxInLocal.upperCorner
        = max(bboxInLocal.upperCorner, cornerInLocal);
    }
    return bboxInLocal;
}

inline V2d Transform2::toWorld(const V2d& pointInLocal) const {
    // Convert to the world frame
    return V2d(_cosAngle * pointInLocal.x - _sinAngle * pointInLocal.y
               + _translation.x,
               _sinAngle * pointInLocal.x + _cosAngle * pointInLocal.y
               + _translation.y);
}

inline V2d Transform2::toWorldDirection(const V2d& dirInLocal) const {
    // Convert to the world frame
    return V2d(_cosAngle * dirInLocal.x - _sinAngle * dirInLocal.y,
               _sinAngle * dirInLocal.x + _cosAngle * dirInLocal.y);
}

inline Ray2d Transform2::toWorld(const Ray2d& rayInLocal) const {
    return Ray2d(toWorld(rayInLocal.origin),
                 toWorldDirection(rayInLocal.direction));
}

inline BoundingBox2d Transform2::toWorld(const BoundingBox2d& bboxInLocal) const {
    BoundingBox2d bboxInWorld;
    for (int i = 0; i < 4; ++i) {
        auto cornerInWorld = toWorld(bboxInLocal.corner(i));
        bboxInWorld.lowerCorner
        = min(bboxInWorld.lowerCorner, cornerInWorld);
        bboxInWorld.upperCorner
        = max(bboxInWorld.upperCorner, cornerInWorld);
    }
    return bboxInWorld;
}

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#endif  // INCLUDE_JET_DETAIL_TRANSFORM2_INL_H_
