//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_subpass.h"
#include "rendering/render_pass.h"
#include "lighting/shadow_manager.h"
#include "renderer.h"
#include "camera.h"

namespace vox {
ShadowSubpass::ShadowSubpass(View* view,
                             Scene* scene,
                             Camera* camera):
Subpass(view, scene, camera) {
}

void ShadowSubpass::setViewProjectionMatrix(const Matrix4x4F& vp) {
    _vp = vp;
}

void ShadowSubpass::prepare() {
#pragma mark Shadow pass render pipeline setup
    {
        _shadowGenDescriptor.label("Shadow Gen");
        _shadowGenDescriptor.fragmentFunction(nullptr);
        _shadowGenDescriptor.depthAttachmentPixelFormat(ShadowManager::SHADOW_MAP_FORMAT);
    }
    
#pragma mark Shadow pass depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Shadow Gen");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLessEqual);
        depthStencilDesc.depthWriteEnabled(true);
        _shadowDepthStencilState = _scene->device()->makeDepthStencilState(depthStencilDesc);
    }
}

void ShadowSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.label("Shadow Map Pass");
    commandEncoder.setDepthStencilState(_shadowDepthStencilState);
    commandEncoder.setCullMode(MTL::CullModeFront);
    commandEncoder.setDepthBias(0.015, 7, 0.02);
    commandEncoder.setVertexBytes(_vp.data(), sizeof(Matrix4x4F), 11);
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
    
    for (auto &element : opaqueQueue) {
        auto macros = compileMacros;
        auto renderer = element.renderer;
        if (renderer->castShadow) {
            renderer->shaderData.mergeMacro(macros, macros);
            renderEncoder.setVertexBytes(renderer->entity()->transform->worldMatrix().data(), sizeof(Matrix4x4F), 12);

            ShaderProgram *program = _pass->resourceCache().requestShader(_pass->library(), "vertex_depth", "", macros);
            if (!program->isValid()) {
                continue;
            }
            _shadowGenDescriptor.vertexFunction(program->vertexShader());
            
            // manully
            auto& mesh = element.mesh;
            _shadowGenDescriptor.vertexDescriptor(&mesh->vertexDescriptor());
            auto _shadowGenPipelineState = _pass->resourceCache().requestRenderPipelineState(_shadowGenDescriptor);
            renderEncoder.setRenderPipelineState(*_shadowGenPipelineState);

            auto& submesh = element.subMesh;
            for (auto &meshBuffer: mesh->vertexBuffers()) {
                renderEncoder.setVertexBuffer(meshBuffer.buffer(),
                                              meshBuffer.offset(),
                                              meshBuffer.argumentIndex());
            }
            renderEncoder.drawIndexedPrimitives(submesh->primitiveType(),
                                                submesh->indexCount(),
                                                submesh->indexType(),
                                                submesh->indexBuffer().buffer(),
                                                submesh->indexBuffer().offset());
        }
    }
}

}
