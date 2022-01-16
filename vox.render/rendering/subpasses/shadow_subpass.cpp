//
//  shadow_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "shadow_subpass.h"
#include "material/material.h"
// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
ShadowSubpass::ShadowSubpass(MTL::RenderPassDescriptor* desc,
                             Scene* scene,
                             Camera* camera,
                             MTL::Library& shaderLibrary,
                             MTL::Device& m_device):
Subpass(desc, m_device, scene, camera) {
    CFErrorRef error = nullptr;
    
#pragma mark Shadow pass render pipeline setup
    {
        MTL::Function *shadowVertexFunction = shaderLibrary.newFunctionWithName("shadow_vertex");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        renderPipelineDescriptor.label("Shadow Gen");
        renderPipelineDescriptor.vertexDescriptor(nullptr);
        renderPipelineDescriptor.vertexFunction(shadowVertexFunction);
        renderPipelineDescriptor.fragmentFunction(nullptr);
        renderPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
        
        m_shadowGenPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
        
        delete shadowVertexFunction;
    }
    
#pragma mark Shadow pass depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Shadow Gen");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLessEqual);
        depthStencilDesc.depthWriteEnabled(true);
        m_shadowDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
    }
}

void ShadowSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.label("Shadow Map Pass");
    
    commandEncoder.setRenderPipelineState(m_shadowGenPipelineState);
    commandEncoder.setDepthStencilState(m_shadowDepthStencilState);
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setDepthBias(0.015, 7, 0.02);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    
    drawMeshes(commandEncoder);
}

void ShadowSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
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
