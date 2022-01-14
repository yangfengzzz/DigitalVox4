//
//  sub_mesh.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef sub_mesh_hpp
#define sub_mesh_hpp

#include "mesh_buffer.h"

namespace vox {
// App specific submesh class containing data to draw a submesh
struct Submesh {
public:
    Submesh(MTL::PrimitiveType primitiveType,
            MTL::IndexType indexType,
            MTL::UInteger indexCount,
            const MeshBuffer indexBuffer,
            const std::vector<MTL::Texture> &textures);
    
    Submesh(MTL::PrimitiveType primitiveType,
            MTL::IndexType indexType,
            MTL::UInteger indexCount,
            MeshBuffer indexBuffer);
    
    Submesh(const Submesh &rhs) = default;
    
    Submesh(Submesh &&rhs) = default;
    
    Submesh &operator=(Submesh &rhs) = default;
    
    Submesh &operator=(Submesh &&rhs) = default;
    
    virtual ~Submesh();
    
    MTL::PrimitiveType primitiveType() const;
    
    MTL::IndexType indexType() const;
    
    MTL::UInteger indexCount() const;
    
    const MeshBuffer &indexBuffer() const;
    
    const std::vector<MTL::Texture> &textures() const;
    
private:
    
    MTL::PrimitiveType m_primitiveType;
    
    MTL::IndexType m_indexType;
    
    MTL::UInteger m_indexCount;
    
    MeshBuffer m_indexBuffer;
    
    std::vector<MTL::Texture> m_textures;
};

inline MTL::PrimitiveType Submesh::primitiveType() const {
    return m_primitiveType;
}

inline MTL::IndexType Submesh::indexType() const {
    return m_indexType;
}

inline MTL::UInteger Submesh::indexCount() const {
    return m_indexCount;
}

inline const MeshBuffer &Submesh::indexBuffer() const {
    return m_indexBuffer;
}

inline const std::vector<MTL::Texture> &Submesh::textures() const {
    return m_textures;
}

}

#endif /* sub_mesh_hpp */
