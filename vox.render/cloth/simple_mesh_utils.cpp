//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simple_mesh_utils.h"

namespace vox {
namespace cloth {
uint32_t generateConvexPolyhedronPlanes(int segmentsX, int segmentsY, physx::PxVec3 center,
                                        float radius, std::vector<physx::PxVec4> *planes) {
    int offset = 0;
    if (planes) {
        planes->reserve(planes->size() + segmentsX * segmentsY);
        offset = (int) planes->size();
    }
    
    segmentsY += 1;
    for (int i = 1; i < segmentsY; i++) {
        float angleY = (float) i / (float) segmentsY * physx::PxPi + physx::PxPiDivTwo;
        for (int j = 0; j < segmentsX; j++) {
            float angleX = (float) j / (float) segmentsX * physx::PxTwoPi;
            
            physx::PxVec3 nx(cosf(angleX), 0.0f, sinf(angleX));
            physx::PxVec3 n = cosf(angleY) * nx + sinf(angleY) * physx::PxVec3(0.0f, 1.0f, 0.0f);
            
            physx::PxVec3 p = n * radius + center;
            
            if (planes) planes->push_back(constructPlaneFromPointNormal(p, n));
        }
    }
    uint64_t shift = (segmentsX * (segmentsY - 1) + offset);
    uint64_t excludeMask = (((uint64_t) 1 << offset) - 1);
    uint64_t mask = (((uint64_t) 1 << shift) - 1) & ~excludeMask;
    return static_cast<uint32_t>(mask);
}


}
}
