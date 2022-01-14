//
//  mesh.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "mesh.h"

namespace vox {
Mesh::Mesh() {
    // Construct a mesh with no submeshes and no vertexBuffer
}


Mesh::Mesh(const std::vector<Submesh> &submeshes,
           const std::vector<MeshBuffer> &vertexBuffers)
: m_submeshes(submeshes), m_vertexBuffers(vertexBuffers) {
    // Member initialization only
}


Mesh::Mesh(const Submesh &submesh,
           const std::vector<MeshBuffer> &vertexBuffers)
: m_vertexBuffers(vertexBuffers) {
    m_submeshes.emplace_back(submesh);
}

Mesh::~Mesh() {
}

}
