//
//  sub_mesh.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "sub_mesh.h"

namespace vox {
// Initialize a submesh without textures
Submesh::Submesh(MTL::PrimitiveType primitiveType,
                 MTL::IndexType indexType,
                 MTL::UInteger indexCount,
                 MeshBuffer indexBuffer)
: m_primitiveType(primitiveType), m_indexType(indexType),
m_indexCount(indexCount), m_indexBuffer(indexBuffer) {
    
}

Submesh::~Submesh() {
}

}
