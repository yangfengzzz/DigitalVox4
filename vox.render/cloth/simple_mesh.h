//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef simple_mesh_hpp
#define simple_mesh_hpp

#include <vector>
#include <foundation/PxVec2.h>
#include <foundation/PxVec3.h>

namespace vox {
namespace cloth {

class Mesh {
    virtual uint32_t getVertexStride() const = 0;
};

/**
 * SimpleMesh: position + normal + uv
 * We use only this type everywhere, once other versions will be required we should generalize Mesh and refactor code.
 */
class SimpleMesh : public Mesh {
public:
    
    class Vertex {
    public:
        physx::PxVec3 position;
        physx::PxVec3 normal;
        physx::PxVec2 uv;
    };
    
    virtual uint32_t getVertexStride() const {
        return sizeof(Vertex);
    }
    
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    
    physx::PxVec3 extents;
    physx::PxVec3 center;
};

}
}
#endif /* mesh_hpp */
