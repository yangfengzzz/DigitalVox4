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
    Transform3(const V3d& translation, const QuaternionD& orientation);
    
    //! Returns the translation.
    const V3d& translation() const;
    
    //! Sets the traslation.
    void setTranslation(const V3d& translation);
    
    //! Returns the orientation.
    const QuaternionD& orientation() const;
    
    //! Sets the orientation.
    void setOrientation(const QuaternionD& orientation);
    
    //! Transforms a point in world coordinate to the local frame.
    V3d toLocal(const V3d& pointInWorld) const;
    
    //! Transforms a direction in world coordinate to the local frame.
    V3d toLocalDirection(const V3d& dirInWorld) const;
    
    //! Transforms a ray in world coordinate to the local frame.
    Ray3D toLocal(const Ray3D& rayInWorld) const;
    
    //! Transforms a bounding box in world coordinate to the local frame.
    BoundingBox3D toLocal(const BoundingBox3D& bboxInWorld) const;
    
    //! Transforms a point in local space to the world coordinate.
    V3d toWorld(const V3d& pointInLocal) const;
    
    //! Transforms a direction in local space to the world coordinate.
    V3d toWorldDirection(const V3d& dirInLocal) const;
    
    //! Transforms a ray in local space to the world coordinate.
    Ray3D toWorld(const Ray3D& rayInLocal) const;
    
    //! Transforms a bounding box in local space to the world coordinate.
    BoundingBox3D toWorld(const BoundingBox3D& bboxInLocal) const;
    
private:
    V3d _translation = V3d(0);
    QuaternionD _orientation;
    Eigen::Matrix3d _orientationMat3;
    Eigen::Matrix3d _inverseOrientationMat3;
};

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#include "transform3-inl.h"

#endif  // INCLUDE_JET_TRANSFORM3_H_
