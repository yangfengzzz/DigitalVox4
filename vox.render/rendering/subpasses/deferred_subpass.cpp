//
//  deferred_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "deferred_subpass.h"
#include "material/material.h"

#include "core/CPPMetalAssert.hpp"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
DeferredSubpass::DeferredSubpass(MTL::RenderPassDescriptor* desc,
                                 Scene* scene,
                                 MTL::Library& shaderLibrary,
                                 MTL::Device& m_device,
                                 MTL::VertexDescriptor& m_defaultVertexDescriptor,
                                 MTL::RenderPassDescriptor* shadow_desc):
Subpass(desc, scene),
shadow_desc(shadow_desc) {
    CFErrorRef error = nullptr;

    {
        MTL::Function GBufferVertexFunction = shaderLibrary.makeFunction("gbuffer_vertex");
        MTL::Function GBufferFragmentFunction = shaderLibrary.makeFunction("gbuffer_fragment");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        
        renderPipelineDescriptor.label("G-buffer Creation");
        renderPipelineDescriptor.vertexDescriptor(&m_defaultVertexDescriptor);
        
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(MTL::PixelFormatInvalid);
        renderPipelineDescriptor.colorAttachments[RenderTargetAlbedo].pixelFormat(desc->colorAttachments[RenderTargetAlbedo].texture().pixelFormat());
        renderPipelineDescriptor.colorAttachments[RenderTargetNormal].pixelFormat(desc->colorAttachments[RenderTargetNormal].texture().pixelFormat());
        renderPipelineDescriptor.colorAttachments[RenderTargetDepth].pixelFormat(desc->colorAttachments[RenderTargetDepth].texture().pixelFormat());
        renderPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(desc->stencilAttachment.texture().pixelFormat());
        
        renderPipelineDescriptor.vertexFunction(&GBufferVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&GBufferFragmentFunction);
        
        m_GBufferPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
        
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
    commandEncoder.setRenderPipelineState(m_GBufferPipelineState);
    commandEncoder.setDepthStencilState(m_GBufferDepthStencilState);
    commandEncoder.setStencilReferenceValue(128);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentTexture(shadow_desc->depthAttachment.texture(), TextureIndexShadow);
    
    drawMeshes(commandEncoder);
    commandEncoder.popDebugGroup();
}

void DeferredSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    scene->_componentsManager.callRender(opaqueQueue, alphaTestQueue, transparentQueue);
    
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
