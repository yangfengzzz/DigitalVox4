// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef INCLUDE_JET_DETAIL_TRANSFORM3_INL_H_
#define INCLUDE_JET_DETAIL_TRANSFORM3_INL_H_

#include "transform3.h"

#include <algorithm>
#include <cmath>

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

inline Transform3::Transform3() {
}

inline Transform3::Transform3(const V3d& translation,
                              const Quatd& orientation) {
    setTranslation(translation);
    setOrientation(orientation);
}

inline const V3d& Transform3::translation() const {
    return _translation;
}

inline void Transform3::setTranslation(const V3d& translation) {
    _translation = translation;
}

inline const Quatd& Transform3::orientation() const {
    return _orientation;
}

inline void Transform3::setOrientation(const Quatd& orientation) {
    _orientation = orientation;
    _orientationMat4 = orientation.toMatrix44();
    _inverseOrientationMat4 = orientation.inverse().toMatrix44();
}

inline V3d Transform3::toLocal(const V3d& pointInWorld) const {
    V3d result;
    _inverseOrientationMat4.multDirMatrix(pointInWorld - _translation, result);
    return result;
}

inline V3d Transform3::toLocalDirection(const V3d& dirInWorld) const {
    V3d result;
    _inverseOrientationMat4.multDirMatrix(dirInWorld, result);
    return result;
}

inline Ray3d Transform3::toLocal(const Ray3d& rayInWorld) const {
    return Ray3d(
                 toLocal(rayInWorld.origin),
                 toLocalDirection(rayInWorld.direction));
}

inline BoundingBox3d Transform3::toLocal(const BoundingBox3d& bboxInWorld) const {
    BoundingBox3d bboxInLocal;
    for (int i = 0; i < 8; ++i) {
        auto cornerInLocal = toLocal(bboxInWorld.corner(i));
        bboxInLocal.lowerCorner
        = min(bboxInLocal.lowerCorner, cornerInLocal);
        bboxInLocal.upperCorner
        = max(bboxInLocal.upperCorner, cornerInLocal);
    }
    return bboxInLocal;
}

inline V3d Transform3::toWorld(const V3d& pointInLocal) const {
    V3d result;
    _orientationMat4.multDirMatrix(pointInLocal, result);
    return result + _translation;
}

inline V3d Transform3::toWorldDirection(const V3d& dirInLocal) const {
    V3d result;
    _orientationMat4.multDirMatrix(dirInLocal, result);
    return result;    
}

inline Ray3d Transform3::toWorld(const Ray3d& rayInLocal) const {
    return Ray3d(toWorld(rayInLocal.origin),
                 toWorldDirection(rayInLocal.direction));
}

inline BoundingBox3d Transform3::toWorld(const BoundingBox3d& bboxInLocal) const {
    BoundingBox3d bboxInWorld;
    for (int i = 0; i < 8; ++i) {
        auto cornerInWorld = toWorld(bboxInLocal.corner(i));
        bboxInWorld.lowerCorner
        = min(bboxInWorld.lowerCorner, cornerInWorld);
        bboxInWorld.upperCorner
        = max(bboxInWorld.upperCorner, cornerInWorld);
    }
    return bboxInWorld;
}

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#endif  // INCLUDE_JET_DETAIL_TRANSFORM3_INL_H_
