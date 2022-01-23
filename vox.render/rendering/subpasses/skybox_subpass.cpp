//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "skybox_subpass.h"
#include "rendering/render_pass.h"
#include "mesh/primitive_mesh.h"
#include "camera.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(View* view,
                             Scene* scene,
                             Camera* camera):
Subpass(view, scene, camera) {
}

void SkyboxSubpass::createSphere(float radius) {
    _mesh = PrimitiveMesh::createSphere(&_view->device(), radius);
    _type = SkyBoxType::Sphere;
}

void SkyboxSubpass::createCuboid() {
    _mesh = PrimitiveMesh::createCuboid(&_view->device(), 1, 1, 1);
    _type = SkyBoxType::Cuboid;
}

std::shared_ptr<MTL::Texture> SkyboxSubpass::textureCubeMap() {
    return _cubeMap;
}

void SkyboxSubpass::setTextureCubeMap(std::shared_ptr<MTL::Texture> v) {
    _cubeMap = v;
}

//MARK: - Render
void SkyboxSubpass::prepare() {
#pragma mark Sky render pipeline setup
    {
        _skyboxPipelineDescriptor.label("Sky");
        _skyboxPipelineDescriptor.vertexDescriptor(&(_mesh->vertexDescriptor()));
        _skyboxPipelineDescriptor.colorAttachments[0].pixelFormat(_view->colorPixelFormat());
        _skyboxPipelineDescriptor.depthAttachmentPixelFormat(_view->depthStencilPixelFormat());
        _skyboxPipelineDescriptor.stencilAttachmentPixelFormat(_view->depthStencilPixelFormat());
        
        if (_type == SkyBoxType::Sphere) {
            MTL::Function skyboxVertexFunction = _pass->library().makeFunction("vertex_sphere_skybox");
            _skyboxPipelineDescriptor.vertexFunction(&skyboxVertexFunction);
        } else {
            MTL::Function skyboxVertexFunction = _pass->library().makeFunction("vertex_cube_skybox");
            _skyboxPipelineDescriptor.vertexFunction(&skyboxVertexFunction);
        }
        MTL::Function skyboxFragmentFunction = _pass->library().makeFunction("fragment_skybox");
        _skyboxPipelineDescriptor.fragmentFunction(&skyboxFragmentFunction);
    }
    
#pragma mark Post lighting depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Less -Writes");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLessEqual);
        depthStencilDesc.depthWriteEnabled(false);
        
        _dontWriteDepthStencilState = _view->device().makeDepthStencilState(depthStencilDesc);
    }
}

void SkyboxSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Sky");
    auto _skyboxPipelineState = _pass->resourceCache().requestRenderPipelineState(_skyboxPipelineDescriptor);
    commandEncoder.setRenderPipelineState(*_skyboxPipelineState);
    commandEncoder.setDepthStencilState(_dontWriteDepthStencilState);
    commandEncoder.setCullMode(MTL::CullModeBack);
    
    const auto projectionMatrix = _camera->projectionMatrix();
    auto viewMatrix = _camera->viewMatrix();
    if (_type == SkyBoxType::Cuboid) {
        viewMatrix[12] = 0;
        viewMatrix[13] = 0;
        viewMatrix[14] = 0;
        viewMatrix[15] = 1;
    }
    auto _matrix = projectionMatrix * viewMatrix;
    commandEncoder.setVertexBytes(_matrix.data(), sizeof(Matrix4x4F), 10);
    commandEncoder.setFragmentTexture(*_cubeMap, 0);
    
    for (auto &meshBuffer: _mesh->vertexBuffers()) {
        commandEncoder.setVertexBuffer(meshBuffer.buffer(),
                                       meshBuffer.offset(),
                                       meshBuffer.argumentIndex());
    }
    
    
    for (auto &submesh: _mesh->submeshes()) {
        commandEncoder.drawIndexedPrimitives(submesh.primitiveType(),
                                             submesh.indexCount(),
                                             submesh.indexType(),
                                             submesh.indexBuffer().buffer(),
                                             submesh.indexBuffer().offset());
    }
    commandEncoder.popDebugGroup();
}

}
