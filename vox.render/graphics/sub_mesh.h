//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
        
private:
    MTL::PrimitiveType m_primitiveType;
    
    MTL::IndexType m_indexType;
    
    MTL::UInteger m_indexCount;
    
    MeshBuffer m_indexBuffer;
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

}

#endif /* sub_mesh_hpp */
