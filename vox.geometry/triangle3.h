//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_TRIANGLE3_H_
#define INCLUDE_VOX_TRIANGLE3_H_

#include "surface3.h"

namespace vox {

//!
//! \brief 3-D triangle geometry.
//!
//! This class represents 3-D triangle geometry which extends Surface3 by
//! overriding surface-related queries.
//!
class Triangle3 final : public Surface3 {
public:
    class Builder;
    
    //! Three points.
    std::array<Point3D, 3> points;
    
    //! Three normals.
    std::array<Vector3D, 3> normals;
    
    //! Three UV coordinates.
    std::array<Vector2D, 3> uvs;
    
    //! Constructs an empty triangle.
    Triangle3(const Transform3D &transform = Transform3D(),
              bool isNormalFlipped = false);
    
    //! Constructs a triangle with given \p points, \p normals, and \p uvs.
    Triangle3(const std::array<Point3D, 3> &points,
              const std::array<Vector3D, 3> &normals,
              const std::array<Vector2D, 3> &uvs,
              const Transform3D &transform = Transform3D(),
              bool isNormalFlipped = false);
    
    //! Copy constructor
    Triangle3(const Triangle3 &other);
    
    //! Returns the area of this triangle.
    double area() const;
    
    //! Returns barycentric coordinates for the given point \p pt.
    void getBarycentricCoords(const Point3D &pt,
                              double *b0,
                              double *b1,
                              double *b2) const;
    
    //! Returns the face normal of the triangle.
    Vector3D faceNormal() const;
    
    //! Set Triangle3::normals to the face normal.
    void setNormalsToFaceNormal();
    
    //! Returns builder fox Triangle3.
    static Builder builder();
    
protected:
    Point3D closestPointLocal(const Point3D &otherPoint) const override;
    
    bool intersectsLocal(const Ray3D &ray) const override;
    
    BoundingBox3D boundingBoxLocal() const override;
    
    Vector3D closestNormalLocal(const Point3D &otherPoint) const override;
    
    SurfaceRayIntersection3 closestIntersectionLocal(const Ray3D &ray) const override;
};

//! Shared pointer for the Triangle3 type.
using Triangle3Ptr = std::shared_ptr<Triangle3>;


//!
//! \brief Front-end to create Triangle3 objects step by step.
//!
class Triangle3::Builder final
: public SurfaceBuilderBase3<Triangle3::Builder> {
public:
    //! Returns builder with points.
    Builder &withPoints(const std::array<Point3D, 3> &points);
    
    //! Returns builder with normals.
    Builder &withNormals(const std::array<Vector3D, 3> &normals);
    
    //! Returns builder with uvs.
    Builder &withUvs(const std::array<Vector2D, 3> &uvs);
    
    //! Builds Triangle3.
    Triangle3 build() const;
    
    //! Builds shared pointer of Triangle3 instance.
    Triangle3Ptr makeShared() const;
    
private:
    std::array<Point3D, 3> _points;
    std::array<Vector3D, 3> _normals;
    std::array<Vector2D, 3> _uvs;
};

}  // namespace vox

#endif  // INCLUDE_VOX_TRIANGLE3_H_
