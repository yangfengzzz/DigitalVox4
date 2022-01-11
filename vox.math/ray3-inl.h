// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef INCLUDE_JET_DETAIL_RAY3_INL_H_
#define INCLUDE_JET_DETAIL_RAY3_INL_H_

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

template <typename T>
Ray<T, 3>::Ray() : Ray(Vec3<T>(0), Vec3<T>(1, 0, 0)) {
}

template <typename T>
Ray<T, 3>::Ray(const Vec3<T>& newOrigin,
               const Vec3<T>& newDirection) :
origin(newOrigin),
direction(newDirection.normalized()) {
}

template <typename T>
Ray<T, 3>::Ray(const Ray& other) :
origin(other.origin),
direction(other.direction) {
}

template <typename T>
Vec3<T> Ray<T, 3>::pointAt(T t) const {
    return origin + t * direction;
}

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#endif  // INCLUDE_JET_DETAIL_RAY3_INL_H_
