//
//  sub_mesh.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "sub_mesh.h"

namespace vox {
inline Submesh::Submesh(MTL::PrimitiveType primitiveType,
                        MTL::IndexType indexType,
                        MTL::UInteger indexCount,
                        const MeshBuffer indexBuffer,
                        const std::vector<MTL::Texture> &textures)
: m_primitiveType(primitiveType),
m_indexType(indexType),
m_indexCount(indexCount),
m_indexBuffer(indexBuffer),
m_textures(textures) {
    // Member initialization only
}

// Initialize a submesh without textures
inline Submesh::Submesh(MTL::PrimitiveType primitiveType,
                        MTL::IndexType indexType,
                        MTL::UInteger indexCount,
                        MeshBuffer indexBuffer)
: m_primitiveType(primitiveType), m_indexType(indexType), m_indexCount(indexCount), m_indexBuffer(indexBuffer) {
    
}

Submesh::~Submesh() {
}

}
