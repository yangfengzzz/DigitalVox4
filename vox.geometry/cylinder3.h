// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef INCLUDE_JET_CYLINDER3_H_
#define INCLUDE_JET_CYLINDER3_H_

#include "surface3.h"

namespace vox {

//!
//! \brief 3-D cylinder geometry.
//!
//! This class represents 3-D cylinder geometry which extends Surface3 by
//! overriding surface-related queries. The cylinder is aligned with the y-axis.
//!
class Cylinder3 final : public Surface3 {
public:
    class Builder;
    
    //! Center of the cylinder.
    Point3D center;
    
    //! Radius of the cylinder.
    double radius = 1.0;
    
    //! Height of the cylinder.
    double height = 1.0;
    
    //! Constructs a cylinder with
    Cylinder3(const Transform3D& transform = Transform3D(),
              bool isNormalFlipped = false);
    
    //! Constructs a cylinder with \p center, \p radius, and \p height.
    Cylinder3(const Point3D& center,
              double radius,
              double height,
              const Transform3D& transform = Transform3D(),
              bool isNormalFlipped = false);
    
    //! Copy constructor.
    Cylinder3(const Cylinder3& other);
    
    //! Returns builder fox Cylinder3.
    static Builder builder();
    
protected:
    // Surface3 implementations
    
    Point3D closestPointLocal(const Point3D& otherPoint) const override;
    
    double closestDistanceLocal(const Point3D& otherPoint) const override;
    
    bool intersectsLocal(const Ray3D& ray) const override;
    
    BoundingBox3D boundingBoxLocal() const override;
    
    Vector3D closestNormalLocal(const Point3D& otherPoint) const override;
    
    SurfaceRayIntersection3 closestIntersectionLocal(const Ray3D& ray) const override;
};

//! Shared pointer type for the Cylinder3.
using Cylinder3Ptr = std::shared_ptr<Cylinder3>;


//!
//! \brief Front-end to create Cylinder3 objects step by step.
//!
class Cylinder3::Builder final
: public SurfaceBuilderBase3<Cylinder3::Builder> {
public:
    //! Returns builder with center.
    Builder& withCenter(const Point3D& center);
    
    //! Returns builder with radius.
    Builder& withRadius(double radius);
    
    //! Returns builder with height.
    Builder& withHeight(double height);
    
    //! Builds Cylinder3.
    Cylinder3 build() const;
    
    //! Builds shared pointer of Cylinder3 instance.
    Cylinder3Ptr makeShared() const;
    
private:
    Point3D _center{0, 0, 0};
    double _radius = 1.0;
    double _height = 1.0;
};

}  // namespace jet


#endif  // INCLUDE_JET_CYLINDER3_H_
