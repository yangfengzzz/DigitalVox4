//
//  forward_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "forward_subpass.h"
#include "material/material.h"
#include "graphics/mesh.h"

#include "core/CPPMetalAssert.hpp"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
ForwardSubpass::ForwardSubpass(MTL::RenderPassDescriptor* desc,
                               Scene* scene,
                               MTL::Library& shaderLibrary,
                               MTL::Device& m_device,
                               MTL::PixelFormat colorPixelFormat):
Subpass(desc, m_device, scene) {
    CFErrorRef error = nullptr;
    
    {
        MTL::Function forwardVertexFunction = shaderLibrary.makeFunction("forward_vertex");
        MTL::Function forwardFragmentFunction = shaderLibrary.makeFunction("forward_fragment");
        
        m_forwardPipelineDescriptor.label("G-buffer Creation");
        m_forwardPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(colorPixelFormat);
        m_forwardPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
        m_forwardPipelineDescriptor.stencilAttachmentPixelFormat(desc->stencilAttachment.texture().pixelFormat());
        m_forwardPipelineDescriptor.vertexFunction(&forwardVertexFunction);
        m_forwardPipelineDescriptor.fragmentFunction(&forwardFragmentFunction);
        
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
        
        m_forwardDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
    }
}

void ForwardSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    
    commandEncoder.pushDebugGroup("Draw G-Buffer");
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setDepthStencilState(m_forwardDepthStencilState);
    commandEncoder.setStencilReferenceValue(128);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    
    drawMeshes(commandEncoder);
    commandEncoder.popDebugGroup();
}

void ForwardSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    Imath::M44f viewMat;
    Imath::M44f projMat;
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
        m_forwardPipelineDescriptor.vertexDescriptor(&mesh->vertexDescriptor());
        auto m_forwardPipelineState = m_device.resourceCache().requestRenderPipelineState(m_forwardPipelineDescriptor);
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
