//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_subpass.h"
#include "rendering/render_pass.h"
#include "material/material.h"
#include "graphics/mesh.h"
#include "renderer.h"
#include "camera.h"

namespace vox {
ForwardSubpass::ForwardSubpass(View* view,
                               Scene* scene,
                               Camera* camera):
Subpass(view, scene, camera) {
}

void ForwardSubpass::prepare() {
    _forwardPipelineDescriptor.label("Forward Pipeline");
    _forwardPipelineDescriptor.colorAttachments[0].pixelFormat(_view->colorPixelFormat());
    _forwardPipelineDescriptor.depthAttachmentPixelFormat(_view->depthStencilPixelFormat());
    _forwardPipelineDescriptor.stencilAttachmentPixelFormat(_view->depthStencilPixelFormat());
}

void ForwardSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw G-Buffer");    
    _drawMeshes(commandEncoder);
    commandEncoder.popDebugGroup();
}

void ForwardSubpass::_drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    auto compileMacros = ShaderMacroCollection();
    _scene->shaderData.mergeMacro(compileMacros, compileMacros);
    _camera->shaderData.mergeMacro(compileMacros, compileMacros);
    
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    _scene->_componentsManager.callRender(_camera, opaqueQueue, alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);
    
    _drawElement(renderEncoder, opaqueQueue, compileMacros);
    _drawElement(renderEncoder, alphaTestQueue, compileMacros);
    _drawElement(renderEncoder, transparentQueue, compileMacros);
}

void ForwardSubpass::_drawElement(MTL::RenderCommandEncoder &renderEncoder,
                                  const std::vector<RenderElement> &items,
                                  const ShaderMacroCollection& compileMacros) {
    for (auto &element : items) {
        auto macros = compileMacros;
        auto renderer = element.renderer;
        uint32_t shadowCount = std::any_cast<uint32_t>(_scene->shaderData.getData("u_shadowCount"));
        if (renderer->receiveShadow && shadowCount != 0) {
            renderer->shaderData.enableMacro(SHADOW_MAP_COUNT, std::make_pair(shadowCount, MTL::DataTypeInt));
        }
        uint32_t cubeShadowCount = std::any_cast<uint32_t>(_scene->shaderData.getData("u_cubeShadowCount"));
        if (renderer->receiveShadow && cubeShadowCount != 0) {
            renderer->shaderData.enableMacro(CUBE_SHADOW_MAP_COUNT, std::make_pair(cubeShadowCount, MTL::DataTypeInt));
        }
        renderer->shaderData.mergeMacro(macros, macros);
        
        auto material = element.material;
        material->shaderData.mergeMacro(macros, macros);
        ShaderProgram *program = _pass->resourceCache().requestShader(_pass->library(), material->shader->vertexSource(),
                                                                      material->shader->fragmentSource(), macros);
        if (!program->isValid()) {
            continue;
        }
        _forwardPipelineDescriptor.vertexFunction(program->vertexShader());
        _forwardPipelineDescriptor.fragmentFunction(program->fragmentShader());
        
        // manully
        auto& mesh = element.mesh;
        _forwardPipelineDescriptor.vertexDescriptor(&mesh->vertexDescriptor());
        
        MTL::DepthStencilDescriptor depthStencilDesc;
        material->renderState.apply(_forwardPipelineDescriptor, depthStencilDesc, renderEncoder);
        
        auto _forwardDepthStencilState = _pass->resourceCache().requestDepthStencilState(depthStencilDesc);
        auto _forwardPipelineState = _pass->resourceCache().requestRenderPipelineState(_forwardPipelineDescriptor);
        uploadUniforms(renderEncoder, _forwardPipelineState->materialUniformBlock, material->shaderData);
        uploadUniforms(renderEncoder, _forwardPipelineState->rendererUniformBlock, renderer->shaderData);
        uploadUniforms(renderEncoder, _forwardPipelineState->sceneUniformBlock, _scene->shaderData);
        uploadUniforms(renderEncoder, _forwardPipelineState->cameraUniformBlock, _camera->shaderData);
        renderEncoder.setRenderPipelineState(*_forwardPipelineState);
        renderEncoder.setDepthStencilState(*_forwardDepthStencilState);

        for (auto &meshBuffer: mesh->vertexBuffers()) {
            renderEncoder.setVertexBuffer(meshBuffer.buffer(),
                                          meshBuffer.offset(),
                                          meshBuffer.argumentIndex());
        }
        auto& submesh = element.subMesh;
        renderEncoder.drawIndexedPrimitives(submesh->primitiveType(),
                                            submesh->indexCount(),
                                            submesh->indexType(),
                                            submesh->indexBuffer().buffer(),
                                            submesh->indexBuffer().offset());
    }
}

}
