// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef INCLUDE_JET_RAY3_H_
#define INCLUDE_JET_RAY3_H_

#include "ImathVec.h"
#include "ray.h"

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

//!
//! \brief      Class for 2-D ray.
//!
//! \tparam     T     The value type.
//!
template <typename T>
class Ray<T, 3> final {
public:
    static_assert(std::is_floating_point<T>::value,
                  "Ray only can be instantiated with floating point types");
    
    //! The origin of the ray.
    Vec3<T> origin;
    
    //! The direction of the ray.
    Vec3<T> direction;
    
    //! Constructs an empty ray that points (1, 0, 0) from (0, 0, 0).
    Ray();
    
    //! Constructs a ray with given origin and riection.
    Ray(const Vec3<T>& newOrigin, const Vec3<T>& newDirection);
    
    //! Copy constructor.
    Ray(const Ray& other);
    
    //! Returns a point on the ray at distance \p t.
    Vec3<T> pointAt(T t) const;
};

//! Type alias for 3-D ray.
template <typename T>
using Ray3 = Ray<T, 3>;

//! Float-type 3-D ray.
using Ray3F = Ray3<float>;

//! Double-type 3-D ray.
using Ray3D = Ray3<double>;

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#include "ray3-inl.h"

#endif  // INCLUDE_JET_RAY3_H_
