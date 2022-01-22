//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_subpass.h"
#include "rendering/render_pass.h"
#include "renderer.h"

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
        MTL::Function *shadowVertexFunction = _pass->library().newFunctionWithName("vertex_depth");
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        renderPipelineDescriptor.label("Shadow Gen");
        renderPipelineDescriptor.vertexDescriptor(nullptr);
        renderPipelineDescriptor.vertexFunction(shadowVertexFunction);
        renderPipelineDescriptor.fragmentFunction(nullptr);
        renderPipelineDescriptor.depthAttachmentPixelFormat(_pass->renderPassDescriptor()->depthAttachment.texture().pixelFormat());
        _shadowGenPipelineState = _pass->resourceCache().requestRenderPipelineState(renderPipelineDescriptor);
    }
    
#pragma mark Shadow pass depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Shadow Gen");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLessEqual);
        depthStencilDesc.depthWriteEnabled(true);
        _shadowDepthStencilState = _view->device().makeDepthStencilState(depthStencilDesc);
    }
}

void ShadowSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.label("Shadow Map Pass");
    commandEncoder.setRenderPipelineState(*_shadowGenPipelineState);
    commandEncoder.setDepthStencilState(_shadowDepthStencilState);
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setDepthBias(0.015, 7, 0.02);
    commandEncoder.setVertexBytes(_vp.data(), sizeof(Matrix4x4F), 11);
    drawMeshes(commandEncoder);
}

void ShadowSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    _scene->_componentsManager.callRender(BoundingFrustum(_vp), opaqueQueue, alphaTestQueue, transparentQueue);
    
    for (auto &element : opaqueQueue) {
        renderEncoder.setVertexBytes(element.renderer->entity()->transform->worldMatrix().data(), sizeof(Matrix4x4F), 11);

        auto& submesh = element.subMesh;
        auto& mesh = element.mesh;
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
