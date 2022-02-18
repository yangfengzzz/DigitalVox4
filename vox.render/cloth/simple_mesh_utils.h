//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef simple_mesh_utils_hpp
#define simple_mesh_utils_hpp

#include "simple_mesh.h"
#include <foundation/PxVec4.h>

namespace vox {
namespace cloth {
/*
 * returns a PxVec4 containing [x,y,z,d] for plane equation ax + by + cz + d = 0.
 * Where plane contains p and has normal n.
 */
inline physx::PxVec4 constructPlaneFromPointNormal(physx::PxVec3 p, physx::PxVec3 n) {
    n.normalize();
    return physx::PxVec4(n, -p.dot(n));
}

/*
 * returns two vectors in b and c so that [a b c] form a basis.
 * a needs to be a unit vector.
 */
inline void computeBasis(const physx::PxVec3 &a, physx::PxVec3 *b, physx::PxVec3 *c) {
    if (fabsf(a.x) >= 0.57735f)
        *b = physx::PxVec3(a.y, -a.x, 0.0f);
    else
        *b = physx::PxVec3(0.0f, a.z, -a.y);
    
    *b = b->getNormalized();
    *c = a.cross(*b);
}

uint32_t generateConvexPolyhedronPlanes(int segmentsX, int segmentsY, physx::PxVec3 center,
                                        float radius, std::vector<physx::PxVec4> *planes);


}
}

#endif /* simple_mesh_utils_hpp */
