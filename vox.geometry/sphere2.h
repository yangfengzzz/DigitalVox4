// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef INCLUDE_JET_SPHERE2_H_
#define INCLUDE_JET_SPHERE2_H_

#include "surface2.h"
#include "bounding_box2.h"

namespace vox {

//!
//! \brief 2-D sphere geometry.
//!
//! This class represents 2-D sphere geometry which extends Surface2 by
//! overriding surface-related queries.
//!
class Sphere2 final : public Surface2 {
public:
    class Builder;
    
    //! Center of the sphere.
    Point2D center;
    
    //! Radius of the sphere.
    double radius = 1.0;
    
    //! Constructs a sphere with center at (0, 0) and radius of 1.
    Sphere2(const Transform2D& transform = Transform2D(),
            bool isNormalFlipped = false);
    
    //! Constructs a sphere with \p center and \p radius.
    Sphere2(const Point2D& center,
            double radius,
            const Transform2D& transform = Transform2D(),
            bool isNormalFlipped = false);
    
    //! Copy constructor.
    Sphere2(const Sphere2& other);
    
    //! Returns builder fox Sphere2.
    static Builder builder();
    
private:
    Point2D closestPointLocal(const Point2D& otherPoint) const override;
    
    double closestDistanceLocal(const Point2D& otherPoint) const override;
    
    bool intersectsLocal(const Ray2D& ray) const override;
    
    BoundingBox2D boundingBoxLocal() const override;
    
    Vector2D closestNormalLocal(const Point2D& otherPoint) const override;
    
    SurfaceRayIntersection2 closestIntersectionLocal(const Ray2D& ray) const override;
};

//! Shared pointer for the Sphere2 type.
using Sphere2Ptr = std::shared_ptr<Sphere2>;


//!
//! \brief Front-end to create Sphere2 objects step by step.
//!
class Sphere2::Builder final : public SurfaceBuilderBase2<Sphere2::Builder> {
public:
    //! Returns builder with sphere center.
    Builder& withCenter(const Point2D& center);
    
    //! Returns builder with sphere radius.
    Builder& withRadius(double radius);
    
    //! Builds Sphere2.
    Sphere2 build() const;
    
    //! Builds shared pointer of Sphere2 instance.
    Sphere2Ptr makeShared() const;
    
private:
    Point2D _center{0, 0};
    double _radius = 0.0;
};

}  // namespace jet


#endif  // INCLUDE_JET_SPHERE2_H_
