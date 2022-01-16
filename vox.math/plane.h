//
//  plane.h
//  DigitalVox4
//
//  Created by 杨丰 on 2022/1/16.
//

#ifndef plane_h
#define plane_h

#include "vector.h"

namespace vox {

//!
//! \brief      Class for ray.
//!
//! \tparam     T     The value type.
//! \tparam     N     The dimension.
//!
template <typename T, size_t N>
class Plane {
    static_assert(N != 2 && N != 3, "Not implemented.");
    static_assert(std::is_floating_point<T>::value,
                  "Plane only can be instantiated with floating point types");
};

}  // namespace vox

#endif /* plane_h */
