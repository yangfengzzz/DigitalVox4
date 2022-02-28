//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gpu_skinned_mesh_renderer.h"
#include "entity.h"
#include "scene.h"
#include "shader/shader.h"
#include "metal_helpers.h"

namespace vox {
GPUSkinnedMeshRenderer::GPUSkinnedMeshRenderer(Entity *entity) :
MeshRenderer(entity),
_jointMatrixProperty(Shader::createProperty("u_jointMatrix", ShaderDataGroup::Renderer)) {
}

GPUSkinnedMeshRenderer::SkinPtr GPUSkinnedMeshRenderer::skin() {
    return _skin;
}

void GPUSkinnedMeshRenderer::setSkin(const SkinPtr &skin) {
    _skin = skin;
}

void GPUSkinnedMeshRenderer::update(float deltaTime) {
    if (_skin) {
        if (!_hasInitJoints) {
            _initJoints();
            _hasInitJoints = true;
        }
        
        // Update join matrices
        auto m = entity()->transform->worldMatrix();
        auto inverseTransform = m.inverse();
        for (size_t i = 0; i < _skin->joints.size(); i++) {
            auto jointNode = _skin->joints[i];
            auto jointMat = jointNode->transform->worldMatrix() * _skin->inverseBindMatrices[i];
            jointMat = inverseTransform * jointMat;
            std::copy(jointMat.data(), jointMat.data() + 16, _jointMatrix.data() + i * 16);
        }
        memcpy(_matrixPalette->contents(), _jointMatrix.data(), _jointMatrix.size() * sizeof(float));
        shaderData.setData(_jointMatrixProperty, _matrixPalette);
    }
}

void GPUSkinnedMeshRenderer::_initJoints() {
    _jointMatrix.resize(_skin->joints.size() * 16);
    _matrixPalette =
    CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, scene()->device().newBuffer(_jointMatrix.size() * sizeof(float),
                                                                        MTL::ResourceOptionCPUCacheModeDefault));
    shaderData.enableMacro(HAS_SKIN);
}


}
