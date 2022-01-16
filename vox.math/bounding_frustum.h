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
#include "plane3.h"
#include "bounding_box3.h"

namespace vox {

struct BoundingFrustum {
    /** The near plane of this frustum. */
    Plane3F near;
    /** The far plane of this frustum. */
    Plane3F far;
    /** The left plane of this frustum. */
    Plane3F left;
    /** The right plane of this frustum. */
    Plane3F right;
    /** The top plane of this frustum. */
    Plane3F top;
    /** The bottom plane of this frustum. */
    Plane3F bottom;
    
    BoundingFrustum();
    
    /**
     * Constructor of BoundingFrustum.
     * @param matrix - The view-projection matrix
     */
    BoundingFrustum(const Matrix4x4F& matrix);
    
    //! Copy constructor.
    BoundingFrustum(const BoundingFrustum& other);
    
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
    Plane3F getPlane(int index) const;
    
    /**
     * Update all planes from the given matrix.
     * @param matrix - The given view-projection matrix
     */
    void calculateFromMatrix(const Matrix4x4F&matrix);
    
    /**
     * Get whether or not a specified bounding box intersects with this frustum (Contains or Intersects).
     * @param box - The box for testing
     * @returns True if bounding box intersects with this frustum, false otherwise
     */
    bool intersectsBox(const BoundingBox3F &box) const;
};

}
#endif /* bounding_frustum_hpp */
