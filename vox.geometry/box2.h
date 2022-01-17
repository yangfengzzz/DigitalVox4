//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_BOX2_H_
#define INCLUDE_VOX_BOX2_H_

#include "surface2.h"
#include "bounding_box2.h"

namespace vox {

//!
//! \brief 2-D box geometry.
//!
//! This class represents 2-D box geometry which extends Surface2 by overriding
//! surface-related queries. This box implementation is an axis-aligned box
//! that wraps lower-level primitive type, BoundingBox2D.
//!
class Box2 final : public Surface2 {
public:
    class Builder;
    
    //! Bounding box of this box.
    BoundingBox2D bound = BoundingBox2D(Point2D(), Point2D(1.0, 1.0));
    
    //! Constructs (0, 0) x (1, 1) box.
    Box2(const Transform2D &transform = Transform2D(),
         bool isNormalFlipped = false);
    
    //! Constructs a box with given \p lowerCorner and \p upperCorner.
    Box2(const Point2D &lowerCorner,
         const Point2D &upperCorner,
         const Transform2D &transform = Transform2D(),
         bool isNormalFlipped = false);
    
    //! Constructs a box with BoundingBox2D instance.
    Box2(const BoundingBox2D &boundingBox,
         const Transform2D &transform = Transform2D(),
         bool isNormalFlipped = false);
    
    //! Copy constructor.
    Box2(const Box2 &other);
    
    //! Returns builder fox Box2.
    static Builder builder();
    
protected:
    // Surface2 implementations
    
    Point2D closestPointLocal(const Point2D &otherPoint) const override;
    
    bool intersectsLocal(const Ray2D &ray) const override;
    
    BoundingBox2D boundingBoxLocal() const override;
    
    Vector2D closestNormalLocal(const Point2D &otherPoint) const override;
    
    SurfaceRayIntersection2 closestIntersectionLocal(const Ray2D &ray) const override;
};

//! Shared pointer type for the Box2.
using Box2Ptr = std::shared_ptr<Box2>;


//!
//! \brief Front-end to create Box2 objects step by step.
//!
class Box2::Builder final : public SurfaceBuilderBase2<Box2::Builder> {
public:
    //! Returns builder with lower corner set.
    Builder &withLowerCorner(const Point2D &pt);
    
    //! Returns builder with upper corner set.
    Builder &withUpperCorner(const Point2D &pt);
    
    //! Returns builder with bounding box.
    Builder &withBoundingBox(const BoundingBox2D &bbox);
    
    //! Builds Box2.
    Box2 build() const;
    
    //! Builds shared pointer of Box2 instance.
    Box2Ptr makeShared() const;
    
private:
    Point2D _lowerCorner{0, 0};
    Point2D _upperCorner{1, 1};
};

}  // namespace vox


#endif  // INCLUDE_VOX_BOX2_H_
