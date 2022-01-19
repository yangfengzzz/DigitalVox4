//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh.h"

namespace vox {
Mesh::Mesh() {
    // Construct a mesh with no submeshes and no vertexBuffer
}


Mesh::Mesh(const std::vector<Submesh> &submeshes,
           const std::vector<MeshBuffer> &vertexBuffers,
           MTL::VertexDescriptor vertexDescriptor)
: _submeshes(submeshes),
_vertexBuffers(vertexBuffers),
_vertexDescriptor(vertexDescriptor) {
    // Member initialization only
}


Mesh::Mesh(const Submesh &submesh,
           const std::vector<MeshBuffer> &vertexBuffers,
           MTL::VertexDescriptor vertexDescriptor)
: _vertexBuffers(vertexBuffers),
_vertexDescriptor(vertexDescriptor) {
    _submeshes.emplace_back(submesh);
}

Mesh::~Mesh() {
}

}
