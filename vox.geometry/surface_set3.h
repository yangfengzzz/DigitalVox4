//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_SURFACE_SET3_H_
#define INCLUDE_VOX_SURFACE_SET3_H_

#include "bvh3.h"
#include "surface3.h"

#include <vector>

namespace vox {

//!
//! \brief 3-D surface set.
//!
//! This class represents 3-D surface set which extends Surface3 by overriding
//! surface-related queries. This is class can hold a collection of other
//! surface instances.
//!
class SurfaceSet3 final : public Surface3 {
public:
    class Builder;
    
    //! Constructs an empty surface set.
    SurfaceSet3();
    
    //! Constructs with a list of other surfaces.
    explicit SurfaceSet3(const std::vector<Surface3Ptr> &others,
                         const Transform3D &transform = Transform3D(),
                         bool isNormalFlipped = false);
    
    //! Copy constructor.
    SurfaceSet3(const SurfaceSet3 &other);
    
    //! Updates internal spatial query engine.
    void updateQueryEngine() override;
    
    //! Returns true if bounding box can be defined.
    bool isBounded() const override;
    
    //! Returns true if the surface is a valid geometry.
    bool isValidGeometry() const override;
    
    //! Returns the number of surfaces.
    size_t numberOfSurfaces() const;
    
    //! Returns the i-th surface.
    const Surface3Ptr &surfaceAt(size_t i) const;
    
    //! Adds a surface instance.
    void addSurface(const Surface3Ptr &surface);
    
    //! Returns builder for SurfaceSet3.
    static Builder builder();
    
private:
    std::vector<Surface3Ptr> _surfaces;
    std::vector<Surface3Ptr> _unboundedSurfaces;
    mutable Bvh3<Surface3Ptr> _bvh;
    mutable bool _bvhInvalidated = true;
    
    // Surface3 implementations
    
    Point3D closestPointLocal(const Point3D &otherPoint) const override;
    
    BoundingBox3D boundingBoxLocal() const override;
    
    double closestDistanceLocal(const Point3D &otherPoint) const override;
    
    bool intersectsLocal(const Ray3D &ray) const override;
    
    Vector3D closestNormalLocal(const Point3D &otherPoint) const override;
    
    SurfaceRayIntersection3 closestIntersectionLocal(
                                                     const Ray3D &ray) const override;
    
    bool isInsideLocal(const Point3D &otherPoint) const override;
    
    void invalidateBvh();
    
    void buildBvh() const;
};

//! Shared pointer for the SurfaceSet3 type.
using SurfaceSet3Ptr = std::shared_ptr<SurfaceSet3>;

//!
//! \brief Front-end to create SurfaceSet3 objects step by step.
//!
class SurfaceSet3::Builder final
: public SurfaceBuilderBase3<SurfaceSet3::Builder> {
public:
    //! Returns builder with other surfaces.
    Builder &withSurfaces(const std::vector<Surface3Ptr> &others);
    
    //! Builds SurfaceSet3.
    SurfaceSet3 build() const;
    
    //! Builds shared pointer of SurfaceSet3 instance.
    SurfaceSet3Ptr makeShared() const;
    
private:
    std::vector<Surface3Ptr> _surfaces;
};

}  // namespace vox

#endif  // INCLUDE_VOX_SURFACE_SET3_H_
