// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef INCLUDE_JET_TRANSFORM2_H_
#define INCLUDE_JET_TRANSFORM2_H_

#include "bounding_box2.h"
#include "ray2.h"

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

//!
//! \brief Represents 2-D rigid body transform.
//!
class Transform2 {
public:
    //! Constructs identity transform.
    Transform2();
    
    //! Constructs a transform with translation and orientation.
    Transform2(const V2d& translation, double orientation);
    
    //! Returns the translation.
    const V2d& translation() const;
    
    //! Sets the traslation.
    void setTranslation(const V2d& translation);
    
    //! Returns the orientation in radians.
    double orientation() const;
    
    //! Sets the orientation in radians.
    void setOrientation(double orientation);
    
    //! Transforms a point in world coordinate to the local frame.
    V2d toLocal(const V2d& pointInWorld) const;
    
    //! Transforms a direction in world coordinate to the local frame.
    V2d toLocalDirection(const V2d& dirInWorld) const;
    
    //! Transforms a ray in world coordinate to the local frame.
    Ray2d toLocal(const Ray2d& rayInWorld) const;
    
    //! Transforms a bounding box in world coordinate to the local frame.
    BoundingBox2d toLocal(const BoundingBox2d& bboxInWorld) const;
    
    //! Transforms a point in local space to the world coordinate.
    V2d toWorld(const V2d& pointInLocal) const;
    
    //! Transforms a direction in local space to the world coordinate.
    V2d toWorldDirection(const V2d& dirInLocal) const;
    
    //! Transforms a ray in local space to the world coordinate.
    Ray2d toWorld(const Ray2d& rayInLocal) const;
    
    //! Transforms a bounding box in local space to the world coordinate.
    BoundingBox2d toWorld(const BoundingBox2d& bboxInLocal) const;
    
private:
    V2d _translation = V2d(0);
    double _orientation = 0.0;
    double _cosAngle = 1.0;
    double _sinAngle = 0.0;
};

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#include "transform2-inl.h"

#endif  // INCLUDE_JET_TRANSFORM2_H_
