// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef INCLUDE_JET_TRANSFORM3_H_
#define INCLUDE_JET_TRANSFORM3_H_

#include "bounding_box3.h"
#include "ray3.h"
#include "ImathQuat.h"

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

//!
//! \brief Represents 3-D rigid body transform.
//!
class Transform3 {
public:
    //! Constructs identity transform.
    Transform3();
    
    //! Constructs a transform with translation and orientation.
    Transform3(const V3d& translation, const Quatd& orientation);
    
    //! Returns the translation.
    const V3d& translation() const;
    
    //! Sets the traslation.
    void setTranslation(const V3d& translation);
    
    //! Returns the orientation.
    const Quatd& orientation() const;
    
    //! Sets the orientation.
    void setOrientation(const Quatd& orientation);
    
    //! Transforms a point in world coordinate to the local frame.
    V3d toLocal(const V3d& pointInWorld) const;
    
    //! Transforms a direction in world coordinate to the local frame.
    V3d toLocalDirection(const V3d& dirInWorld) const;
    
    //! Transforms a ray in world coordinate to the local frame.
    Ray3d toLocal(const Ray3d& rayInWorld) const;
    
    //! Transforms a bounding box in world coordinate to the local frame.
    BoundingBox3d toLocal(const BoundingBox3d& bboxInWorld) const;
    
    //! Transforms a point in local space to the world coordinate.
    V3d toWorld(const V3d& pointInLocal) const;
    
    //! Transforms a direction in local space to the world coordinate.
    V3d toWorldDirection(const V3d& dirInLocal) const;
    
    //! Transforms a ray in local space to the world coordinate.
    Ray3d toWorld(const Ray3d& rayInLocal) const;
    
    //! Transforms a bounding box in local space to the world coordinate.
    BoundingBox3d toWorld(const BoundingBox3d& bboxInLocal) const;
    
private:
    V3d _translation = V3d(0);
    Quatd _orientation;
    M44d _orientationMat4;
    M44d _inverseOrientationMat4;
};

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#include "transform3-inl.h"

#endif  // INCLUDE_JET_TRANSFORM3_H_
