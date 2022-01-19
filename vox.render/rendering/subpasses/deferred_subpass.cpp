//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "deferred_subpass.h"
#include "rendering/render_pass.h"

#include "material/material.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

#include <glog/logging.h>

namespace vox {
std::string DeferredSubpass::dependedPassName() {
    return "shadowPass";
}

DeferredSubpass::DeferredSubpass(View* view,
                                 Scene* scene,
                                 Camera* camera):
Subpass(view, scene, camera) {
}

void DeferredSubpass::setRenderPass(RenderPass* pass) {
    auto parentPass = pass->findPass(dependedPassName());
    if (parentPass) {
        Subpass::setRenderPass(pass);
        _shadowMap = &parentPass->renderPassDescriptor()->depthAttachment.texture();
    } else {
        LOG(ERROR) << "depend on shadowPass";
    }
}

void DeferredSubpass::prepare() {
    {
        MTL::Function GBufferVertexFunction = _pass->library().makeFunction("gbuffer_vertex");
        MTL::Function GBufferFragmentFunction = _pass->library().makeFunction("gbuffer_fragment");
        
        const MTL::RenderPassDescriptor* desc = _pass->renderPassDescriptor();
        _GBufferPipelineDescriptor.label("G-buffer Creation");
        _GBufferPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(MTL::PixelFormatInvalid);
        _GBufferPipelineDescriptor.colorAttachments[RenderTargetAlbedo].pixelFormat(desc->colorAttachments[RenderTargetAlbedo].texture().pixelFormat());
        _GBufferPipelineDescriptor.colorAttachments[RenderTargetNormal].pixelFormat(desc->colorAttachments[RenderTargetNormal].texture().pixelFormat());
        _GBufferPipelineDescriptor.colorAttachments[RenderTargetDepth].pixelFormat(desc->colorAttachments[RenderTargetDepth].texture().pixelFormat());
        _GBufferPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
        _GBufferPipelineDescriptor.stencilAttachmentPixelFormat(desc->stencilAttachment.texture().pixelFormat());
        _GBufferPipelineDescriptor.vertexFunction(&GBufferVertexFunction);
        _GBufferPipelineDescriptor.fragmentFunction(&GBufferFragmentFunction);        
    }
    
#pragma mark GBuffer depth state setup
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
        
        _GBufferDepthStencilState = _view->device().makeDepthStencilState(depthStencilDesc);
    }
}

void DeferredSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw G-Buffer");
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setDepthStencilState(_GBufferDepthStencilState);
    commandEncoder.setStencilReferenceValue(128);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentTexture(*_shadowMap, TextureIndexShadow);
    
    drawMeshes(commandEncoder);
    commandEncoder.popDebugGroup();
}

void DeferredSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
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
        _GBufferPipelineDescriptor.vertexDescriptor(&mesh->vertexDescriptor());
        auto m_GBufferPipelineState = _view->device().resourceCache().requestRenderPipelineState(_GBufferPipelineDescriptor);
        renderEncoder.setRenderPipelineState(m_GBufferPipelineState);
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
