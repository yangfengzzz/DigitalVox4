//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
