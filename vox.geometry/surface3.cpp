// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

//#include <pch.h>

#include "surface3.h"

#include <algorithm>

using namespace vox;

Surface3::Surface3(const Transform3& transform_, bool isNormalFlipped_)
: transform(transform_), isNormalFlipped(isNormalFlipped_) {}

Surface3::Surface3(const Surface3& other)
: transform(other.transform), isNormalFlipped(other.isNormalFlipped) {}

Surface3::~Surface3() {}

V3d Surface3::closestPoint(const V3d& otherPoint) const {
    return transform.toWorld(closestPointLocal(transform.toLocal(otherPoint)));
}

BoundingBox3d Surface3::boundingBox() const {
    return transform.toWorld(boundingBoxLocal());
}

bool Surface3::intersects(const Ray3d& ray) const {
    return intersectsLocal(transform.toLocal(ray));
}

double Surface3::closestDistance(const V3d& otherPoint) const {
    return closestDistanceLocal(transform.toLocal(otherPoint));
}

SurfaceRayIntersection3 Surface3::closestIntersection(const Ray3d& ray) const {
    auto result = closestIntersectionLocal(transform.toLocal(ray));
    result.point = transform.toWorld(result.point);
    result.normal = transform.toWorldDirection(result.normal);
    result.normal *= (isNormalFlipped) ? -1.0 : 1.0;
    return result;
}

V3d Surface3::closestNormal(const V3d& otherPoint) const {
    auto result = transform.toWorldDirection(closestNormalLocal(transform.toLocal(otherPoint)));
    result *= (isNormalFlipped) ? -1.0 : 1.0;
    return result;
}

bool Surface3::intersectsLocal(const Ray3d& rayLocal) const {
    auto result = closestIntersectionLocal(rayLocal);
    return result.isIntersecting;
}

void Surface3::updateQueryEngine() {
    // Do nothing
}

bool Surface3::isBounded() const { return true; }

bool Surface3::isValidGeometry() const { return true; }

bool Surface3::isInside(const V3d& otherPoint) const {
    return isNormalFlipped == !isInsideLocal(transform.toLocal(otherPoint));
}

double Surface3::closestDistanceLocal(const V3d& otherPointLocal) const {
    return (otherPointLocal - closestPointLocal(otherPointLocal)).length();
}

bool Surface3::isInsideLocal(const V3d& otherPointLocal) const {
    V3d cpLocal = closestPointLocal(otherPointLocal);
    V3d normalLocal = closestNormalLocal(otherPointLocal);
    return (otherPointLocal - cpLocal).dot(normalLocal) < 0.0;
}
