//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_subpass.h"
#include "rendering/render_pass.h"
#include "material/material.h"
// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
ShadowSubpass::ShadowSubpass(View* view,
                             Scene* scene,
                             Camera* camera):
Subpass(view, scene, camera) {
}

void ShadowSubpass::prepare() {
#pragma mark Shadow pass render pipeline setup
    {
        MTL::Function *shadowVertexFunction = _pass->library().newFunctionWithName("shadow_vertex");
        _shadowGenPipelineDescriptor.label("Shadow Gen");
        _shadowGenPipelineDescriptor.vertexDescriptor(nullptr);
        _shadowGenPipelineDescriptor.vertexFunction(shadowVertexFunction);
        _shadowGenPipelineDescriptor.fragmentFunction(nullptr);
        _shadowGenPipelineDescriptor.depthAttachmentPixelFormat(_pass->renderPassDescriptor()->depthAttachment.texture().pixelFormat());
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
    
    auto _shadowGenPipelineState = _view->device().resourceCache().requestRenderPipelineState(_shadowGenPipelineDescriptor);
    commandEncoder.setRenderPipelineState(_shadowGenPipelineState);
    commandEncoder.setDepthStencilState(_shadowDepthStencilState);
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setDepthBias(0.015, 7, 0.02);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    
    drawMeshes(commandEncoder);
}

void ShadowSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    Matrix4x4F viewMat;
    Matrix4x4F projMat;
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    _scene->_componentsManager.callRender(viewMat, projMat,
                                          opaqueQueue, alphaTestQueue, transparentQueue);
    
    for (auto &element : opaqueQueue) {
        // reflection
        auto& submesh = element.subMesh;
        auto& mat = element.material;
        renderEncoder.setFragmentTexture(*std::any_cast<MTL::TexturePtr>(mat->shaderData.getData("u_diffuseTexture")), TextureIndexBaseColor);
        renderEncoder.setFragmentTexture(*std::any_cast<MTL::TexturePtr>(mat->shaderData.getData("u_normalTexture")), TextureIndexNormal);
        renderEncoder.setFragmentTexture(*std::any_cast<MTL::TexturePtr>(mat->shaderData.getData("u_specularTexture")), TextureIndexSpecular);
        
        // manully
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
