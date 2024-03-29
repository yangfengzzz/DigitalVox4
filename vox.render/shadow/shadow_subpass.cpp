//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_subpass.h"
#include "rendering/render_pass.h"
#include "shadow_manager.h"
#include "renderer.h"
#include "camera.h"
#include "material.h"
#include "metal_helpers.h"

namespace vox {
ShadowSubpass::ShadowSubpass(RenderContext *context,
                             Scene *scene,
                             Camera *camera) :
Subpass(context, scene, camera) {
}

void ShadowSubpass::setViewProjectionMatrix(const Matrix4x4F &vp) {
    _vp = vp;
}

void ShadowSubpass::setViewport(const std::optional<MTL::Viewport> &viewport) {
    _viewport = viewport;
}

void ShadowSubpass::prepare() {
#pragma mark Shadow pass render pipeline setup
    _shadowGenDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::RenderPipelineDescriptor,
                                                      MTL::RenderPipelineDescriptor::alloc()->init());
    {
        _shadowGenDescriptor->setLabel(NS::String::string("Shadow Gen", NS::StringEncoding::UTF8StringEncoding));
        _shadowGenDescriptor->setFragmentFunction(nullptr);
        _shadowGenDescriptor->setDepthAttachmentPixelFormat(ShadowManager::SHADOW_MAP_FORMAT);
    }
    
#pragma mark Shadow pass depth state setup
    {
        auto depthStencilDesc = CLONE_METAL_CUSTOM_DELETER(MTL::DepthStencilDescriptor,
                                                           MTL::DepthStencilDescriptor::alloc()->init());
        depthStencilDesc->setLabel(NS::String::string("Shadow Gen", NS::StringEncoding::UTF8StringEncoding));
        depthStencilDesc->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
        depthStencilDesc->setDepthWriteEnabled(true);
        _shadowDepthStencilState = CLONE_METAL_CUSTOM_DELETER(MTL::DepthStencilState,
                                                              _scene->device().newDepthStencilState(depthStencilDesc.get()));
    }
}

void ShadowSubpass::draw(MTL::RenderCommandEncoder &commandEncoder) {
    commandEncoder.setLabel(NS::String::string("Shadow Map Pass", NS::StringEncoding::UTF8StringEncoding));
    commandEncoder.setDepthStencilState(_shadowDepthStencilState.get());
    commandEncoder.setDepthBias(0.01, 1.0, 0.01);
    commandEncoder.setVertexBytes(_vp.data(), sizeof(Matrix4x4F), 11);
    if (_viewport) {
        commandEncoder.setViewport(*_viewport);
    }
    drawMeshes(commandEncoder);
}

void ShadowSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    auto compileMacros = ShaderMacroCollection();
    _scene->shaderData.mergeMacro(compileMacros, compileMacros);
    _camera->shaderData.mergeMacro(compileMacros, compileMacros);
    
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    _scene->_componentsManager.callRender(BoundingFrustum(_vp), opaqueQueue, alphaTestQueue, transparentQueue);
    
    for (auto &element: opaqueQueue) {
        auto macros = compileMacros;
        auto renderer = element.renderer;
        if (renderer->castShadow) {
            renderer->updateShaderData(_camera->viewMatrix(), _camera->projectionMatrix());
            renderer->shaderData.mergeMacro(macros, macros);
            renderEncoder.setVertexBytes(renderer->entity()->transform->worldMatrix().data(), sizeof(Matrix4x4F), 12);
            
            auto material = element.material;
            material->shaderData.mergeMacro(macros, macros);
            MTL::Function* function = _pass->resourceCache().requestFunction(_pass->library(), "vertex_depth", macros);
            _shadowGenDescriptor->setVertexFunction(function);
            
            // manully
            auto &mesh = element.mesh;
            _shadowGenDescriptor->setVertexDescriptor(mesh->vertexDescriptor().get());
            auto _shadowGenPipelineState = _pass->resourceCache().requestPipelineState(*_shadowGenDescriptor);
            renderEncoder.setRenderPipelineState(&_shadowGenPipelineState->handle());
            
            uint32_t index = 0;
            for (auto &meshBuffer: mesh->vertexBufferBindings()) {
                renderEncoder.setVertexBuffer(meshBuffer.get(),
                                              0, index++);
            }
            auto &submesh = element.subMesh;
            
            if (submesh->indexBuffer()) {
                renderEncoder.drawIndexedPrimitives(submesh->primitiveType(),
                                                    submesh->indexCount(),
                                                    submesh->indexType(),
                                                    submesh->indexBuffer().get(),
                                                    0,
                                                    mesh->instanceCount());
            } else {
                renderEncoder.drawPrimitives(submesh->primitiveType(),
                                             NS::UInteger(0),
                                             submesh->indexCount(),
                                             mesh->instanceCount());
            }
        }
    }
}

}
