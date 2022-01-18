//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh_renderer.h"
#include "graphics/mesh.h"

namespace vox {
MeshRenderer::MeshRenderer(Entity *entity) :
Renderer(entity) {
    
}

void MeshRenderer::setMesh(const MeshPtr &newValue) {
    _mesh = newValue;
}

MeshPtr MeshRenderer::mesh() {
    return _mesh;
}

void MeshRenderer::_render(std::vector<RenderElement> &opaqueQueue,
                           std::vector<RenderElement> &alphaTestQueue,
                           std::vector<RenderElement> &transparentQueue) {
    for (int i = 0; i < _materials.size(); i++) {
        opaqueQueue.push_back(RenderElement(this, _mesh, &_mesh->submeshes()[i], _materials[i]));
    }
}


}
