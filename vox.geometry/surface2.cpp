// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

//#include <pch.h>

#include "surface2.h"

#include <algorithm>

using namespace vox;

Surface2::Surface2(const Transform2& transform_, bool isNormalFlipped_)
: transform(transform_), isNormalFlipped(isNormalFlipped_) {}

Surface2::Surface2(const Surface2& other)
: transform(other.transform), isNormalFlipped(other.isNormalFlipped) {}

Surface2::~Surface2() {}

V2d Surface2::closestPoint(const V2d& otherPoint) const {
    return transform.toWorld(closestPointLocal(transform.toLocal(otherPoint)));
}

BoundingBox2d Surface2::boundingBox() const {
    return transform.toWorld(boundingBoxLocal());
}

bool Surface2::intersects(const Ray2d& ray) const {
    return intersectsLocal(transform.toLocal(ray));
}

double Surface2::closestDistance(const V2d& otherPoint) const {
    return closestDistanceLocal(transform.toLocal(otherPoint));
}

SurfaceRayIntersection2 Surface2::closestIntersection(const Ray2d& ray) const {
    auto result = closestIntersectionLocal(transform.toLocal(ray));
    result.point = transform.toWorld(result.point);
    result.normal = transform.toWorldDirection(result.normal);
    result.normal *= (isNormalFlipped) ? -1.0 : 1.0;
    return result;
}

V2d Surface2::closestNormal(const V2d& otherPoint) const {
    auto result = transform.toWorldDirection(closestNormalLocal(transform.toLocal(otherPoint)));
    result *= (isNormalFlipped) ? -1.0 : 1.0;
    return result;
}

void Surface2::updateQueryEngine() {
    // Do nothing
}

bool Surface2::isBounded() const { return true; }

bool Surface2::isValidGeometry() const { return true; }

bool Surface2::isInside(const V2d& otherPoint) const {
    return isNormalFlipped == !isInsideLocal(transform.toLocal(otherPoint));
}

bool Surface2::intersectsLocal(const Ray2d& rayLocal) const {
    auto result = closestIntersectionLocal(rayLocal);
    return result.isIntersecting;
}

double Surface2::closestDistanceLocal(const V2d& otherPointLocal) const {
    return (otherPointLocal - closestPointLocal(otherPointLocal)).length();
}

bool Surface2::isInsideLocal(const V2d& otherPointLocal) const {
    V2d cpLocal = closestPointLocal(otherPointLocal);
    V2d normalLocal = closestNormalLocal(otherPointLocal);
    return (otherPointLocal - cpLocal).dot(normalLocal) < 0.0;
}
