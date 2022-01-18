//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_subpass.h"
#include "material/material.h"
#include "graphics/mesh.h"
#include "renderer.h"
#include "camera.h"
#include "core/cpp_mtl_assert.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
ForwardSubpass::ForwardSubpass(MTL::RenderPassDescriptor* desc,
                               MTL::Device* device,
                               Scene* scene,
                               Camera* camera,
                               MTL::Library& shaderLibrary,
                               MTL::PixelFormat colorPixelFormat):
Subpass(desc, device, scene, camera) {
    CFErrorRef error = nullptr;
    
    {
        MTL::Function forwardVertexFunction = shaderLibrary.makeFunction("forward_vertex");
        MTL::Function forwardFragmentFunction = shaderLibrary.makeFunction("forward_fragment");
        
        _forwardPipelineDescriptor.label("G-buffer Creation");
        _forwardPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(colorPixelFormat);
        _forwardPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
        _forwardPipelineDescriptor.stencilAttachmentPixelFormat(desc->stencilAttachment.texture().pixelFormat());
        _forwardPipelineDescriptor.vertexFunction(&forwardVertexFunction);
        _forwardPipelineDescriptor.fragmentFunction(&forwardFragmentFunction);
        
        MTLAssert(error == nullptr, error, "Failed to create forward render pipeline state");
    }
    
#pragma mark forward depth state setup
    {
#if LIGHT_STENCIL_CULLING
        MTL::StencilDescriptor stencilStateDesc;
        stencilStateDesc.stencilCompareFunction(MTL::CompareFunctionAlways);
        stencilStateDesc.stencilFailureOperation(MTL::StencilOperationKeep);
        stencilStateDesc.depthFailureOperation(MTL::StencilOperationKeep);
        stencilStateDesc.depthStencilPassOperation(MTL::StencilOperationReplace);
        stencilStateDesc.readMask(0x0);
        stencilStateDesc.writeMask(0xFF);
#else
        MTL::StencilDescriptor stencilStateDesc;
#endif
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("G-buffer Creation");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLess);
        depthStencilDesc.depthWriteEnabled(true);
        depthStencilDesc.frontFaceStencil = stencilStateDesc;
        depthStencilDesc.backFaceStencil = stencilStateDesc;
        
        _forwardDepthStencilState = _device->makeDepthStencilState(depthStencilDesc);
    }
}

void ForwardSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw G-Buffer");
    commandEncoder.setCullMode(MTL::CullModeFront);
    commandEncoder.setDepthStencilState(_forwardDepthStencilState);
    commandEncoder.setStencilReferenceValue(128);
    
    drawMeshes(commandEncoder);
    commandEncoder.popDebugGroup();
}

void ForwardSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    Matrix4x4F viewMat = _camera->viewMatrix();
    Matrix4x4F projMat = _camera->projectionMatrix();
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
        
        auto& renderer = element.renderer;
        auto mvpMat = std::any_cast<Matrix4x4F>(renderer->shaderData.getData("u_MVPMat"));
        renderEncoder.setVertexBytes(&mvpMat, sizeof(Matrix4x4F), 5);
        auto normalMat = std::any_cast<Matrix4x4F>(renderer->shaderData.getData("u_normalMat"));
        renderEncoder.setVertexBytes(&normalMat, sizeof(Matrix4x4F), 6);
        
        // manully
        auto& mesh = element.mesh;
        _forwardPipelineDescriptor.vertexDescriptor(&mesh->vertexDescriptor());
        auto m_forwardPipelineState = _device->resourceCache().requestRenderPipelineState(_forwardPipelineDescriptor);
        renderEncoder.setRenderPipelineState(m_forwardPipelineState);
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
