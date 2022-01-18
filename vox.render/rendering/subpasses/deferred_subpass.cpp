//
//  deferred_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "deferred_subpass.h"
#include "material/material.h"

#include "core/cpp_mtl_Assert.hpp"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
DeferredSubpass::DeferredSubpass(MTL::RenderPassDescriptor* desc,
                                 Scene* scene,
                                 Camera* camera,
                                 MTL::Library& shaderLibrary,
                                 MTL::Device& m_device,
                                 MTL::RenderPassDescriptor* shadow_desc):
Subpass(desc, m_device, scene, camera),
shadow_desc(shadow_desc) {
    CFErrorRef error = nullptr;

    {
        MTL::Function GBufferVertexFunction = shaderLibrary.makeFunction("gbuffer_vertex");
        MTL::Function GBufferFragmentFunction = shaderLibrary.makeFunction("gbuffer_fragment");
                
        m_GBufferPipelineDescriptor.label("G-buffer Creation");
        m_GBufferPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(MTL::PixelFormatInvalid);
        m_GBufferPipelineDescriptor.colorAttachments[RenderTargetAlbedo].pixelFormat(desc->colorAttachments[RenderTargetAlbedo].texture().pixelFormat());
        m_GBufferPipelineDescriptor.colorAttachments[RenderTargetNormal].pixelFormat(desc->colorAttachments[RenderTargetNormal].texture().pixelFormat());
        m_GBufferPipelineDescriptor.colorAttachments[RenderTargetDepth].pixelFormat(desc->colorAttachments[RenderTargetDepth].texture().pixelFormat());
        m_GBufferPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
        m_GBufferPipelineDescriptor.stencilAttachmentPixelFormat(desc->stencilAttachment.texture().pixelFormat());
        m_GBufferPipelineDescriptor.vertexFunction(&GBufferVertexFunction);
        m_GBufferPipelineDescriptor.fragmentFunction(&GBufferFragmentFunction);
                
        MTLAssert(error == nullptr, error, "Failed to create GBuffer render pipeline state");
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
        
        m_GBufferDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
    }
}

void DeferredSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    
    commandEncoder.pushDebugGroup("Draw G-Buffer");
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setDepthStencilState(m_GBufferDepthStencilState);
    commandEncoder.setStencilReferenceValue(128);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentTexture(shadow_desc->depthAttachment.texture(), TextureIndexShadow);
    
    drawMeshes(commandEncoder);
    commandEncoder.popDebugGroup();
}

void DeferredSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    Matrix4x4F viewMat;
    Matrix4x4F projMat;
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    scene->_componentsManager.callRender(viewMat, projMat,
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
        m_GBufferPipelineDescriptor.vertexDescriptor(&mesh->vertexDescriptor());
        auto m_GBufferPipelineState = m_device.resourceCache().requestRenderPipelineState(m_GBufferPipelineDescriptor);
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
