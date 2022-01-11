// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef INCLUDE_JET_BOUNDING_BOX_H_
#define INCLUDE_JET_BOUNDING_BOX_H_

#include "ImathVec.h"
#include "constants.h"
#include "math_utils.h"

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

//!
//! \brief Generic N-D axis-aligned bounding box class.
//!
//! \tparam T - Real number type.
//! \tparam N - Dimension.
//!
template <typename T, size_t N>
class BoundingBox {
public:
    static_assert(N != 2 && N != 3, "Not implemented.");
    static_assert(std::is_floating_point<T>::value,
                  "Ray only can be instantiated with floating point types");
};

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#endif  // INCLUDE_JET_BOUNDING_BOX_H_
