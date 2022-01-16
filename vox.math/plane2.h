//
//  plane2.h
//  DigitalVox4
//
//  Created by 杨丰 on 2022/1/16.
//

#ifndef plane2_h
#define plane2_h

#include "vector2.h"
#include "plane.h"

namespace vox {

//!
//! \brief      Class for 2-D plane.
//!
//! \tparam     T     The value type.
//!
template <typename T>
class Plane<T, 2> final {
public:
    static_assert(std::is_floating_point<T>::value,
                  "Plane only can be instantiated with floating point types");
    
    //! The normal of the plane.
    Vector2<T> normal;
    
    //! The distance of the plane along its normal to the origin.
    T distance;
    
    //! Constructs an empty plane that points (1, 0) from (0, 0).
    Plane();
    
    //! Constructs a plane with given origin and riection.
    Plane(const Vector2<T>& newNormal, const T& newDistance);
    
    /**
     * Calculate the plane that contains the three specified points.
     * @param point0 - The first point
     * @param point1 - The second point
     * @param point2 - The third point
     */
    Plane(const Vector2<T>& point0, const Vector2<T>& point1, const Vector2<T>& point2);
    
    //! Copy constructor.
    Plane(const Plane& other);
    
    /**
     * Normalize the normal vector of this plane.
     * @returns The plane after normalize
     */
    Plane<T, 2> normalized() const;
    
    /**
     * Normalize the normal vector of this plane.
     */
    void normalize();
};

//! Type alias for 2-D plane.
template <typename T> using Plane2 = Plane<T, 2>;

//! Float-type 2-D plane.
using Plane2F = Plane2<float>;

//! Double-type 2-D plane.
using Plane2D = Plane2<double>;

}  // namespace vox

#include "plane2-inl.h"

#endif /* plane2_h */
