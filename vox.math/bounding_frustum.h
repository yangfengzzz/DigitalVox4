//
//  bounding_frustum.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/25.
//

#ifndef bounding_frustum_hpp
#define bounding_frustum_hpp

#include <cassert>
#include <cmath>
#include <optional>
#include "ImathPlane.h"
#include "bounding_box3.h"
#include "ImathSphere.h"

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

struct BoundingFrustum {
    /** The near plane of this frustum. */
    Plane3f near;
    /** The far plane of this frustum. */
    Plane3f far;
    /** The left plane of this frustum. */
    Plane3f left;
    /** The right plane of this frustum. */
    Plane3f right;
    /** The top plane of this frustum. */
    Plane3f top;
    /** The bottom plane of this frustum. */
    Plane3f bottom;
    
    /**
     * Constructor of BoundingFrustum.
     * @param matrix - The view-projection matrix
     */
    BoundingFrustum(std::optional<M44f> matrix = std::nullopt);
    
    /**
     * Get the plane by the given index.
     * 0: near
     * 1: far
     * 2: left
     * 3: right
     * 4: top
     * 5: bottom
     * @param index - The index
     * @returns The plane get
     */
    Plane3f getPlane(int index) const;
    
    /**
     * Update all planes from the given matrix.
     * @param matrix - The given view-projection matrix
     */
    void calculateFromMatrix(const M44f &matrix);
    
    /**
     * Get whether or not a specified bounding box intersects with this frustum (Contains or Intersects).
     * @param box - The box for testing
     * @returns True if bounding box intersects with this frustum, false otherwise
     */
    bool intersectsBox(const BoundingBox3f &box) const;
    
    /**
     * Get whether or not a specified bounding sphere intersects with this frustum (Contains or Intersects).
     * @param sphere - The sphere for testing
     * @returns True if bounding sphere intersects with this frustum, false otherwise
     */
    bool intersectsSphere(const Sphere3f &sphere) const;
};

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT
#endif /* bounding_frustum_hpp */
