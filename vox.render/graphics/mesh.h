//
//  mesh.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef mesh_hpp
#define mesh_hpp

#include "sub_mesh.h"

namespace vox {
struct Mesh {
public:
    Mesh();
    
    Mesh(const std::vector<Submesh> &submeshes,
         const std::vector<MeshBuffer> &vertexBuffers,
         MTL::VertexDescriptor vertexDescriptor);
    
    Mesh(const Submesh &submesh,
         const std::vector<MeshBuffer> &vertexBuffers,
         MTL::VertexDescriptor vertexDescriptor);
    
    Mesh(const Mesh &rhs) = default;
    
    Mesh(Mesh &&rhs) = default;
    
    Mesh &operator=(Mesh &rhs) = default;
    
    Mesh &operator=(Mesh &&rhs) = default;
    
    virtual ~Mesh();
    
    const std::vector<Submesh> &submeshes() const;
    
    const std::vector<MeshBuffer> &vertexBuffers() const;
    
    const MTL::VertexDescriptor& vertexDescriptor() const;
    
private:
    std::vector<Submesh> m_submeshes;
    
    std::vector<MeshBuffer> m_vertexBuffers;
    
    MTL::VertexDescriptor m_vertexDescriptor;
};
using MeshPtr = std::shared_ptr<Mesh>;

inline const std::vector<Submesh> &Mesh::submeshes() const {
    return m_submeshes;
}

inline const std::vector<MeshBuffer> &Mesh::vertexBuffers() const {
    return m_vertexBuffers;
}

inline const MTL::VertexDescriptor& Mesh::vertexDescriptor() const {
    return m_vertexDescriptor;
}


}

#endif /* mesh_hpp */
