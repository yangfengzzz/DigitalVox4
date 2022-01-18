//
//  skybox_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "skybox_subpass.h"
#include "core/cpp_mtl_Assert.hpp"
#include "material/material.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(MTL::RenderPassDescriptor* desc,
                             Scene* scene,
                             Camera* camera,
                             MTL::Library& shaderLibrary,
                             MTL::Device& m_device,
                             MTL::PixelFormat colorPixelFormat):
Subpass(desc, m_device, scene, camera) {
    CFErrorRef error = nullptr;
    
#pragma mark Sky render pipeline setup
    {
        MTL::Function skyboxVertexFunction = shaderLibrary.makeFunction("skybox_vertex");
        MTL::Function skyboxFragmentFunction = shaderLibrary.makeFunction("skybox_fragment");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        renderPipelineDescriptor.label("Sky");
        renderPipelineDescriptor.vertexDescriptor(&(scene->background.sky.mesh->vertexDescriptor()));
        renderPipelineDescriptor.vertexFunction(&skyboxVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&skyboxFragmentFunction);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(colorPixelFormat);
        renderPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(desc->stencilAttachment.texture().pixelFormat());
        
        m_skyboxPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
        
        MTLAssert(error == nullptr, error, "Failed to create skybox render pipeline state: %@");
    }
    
#pragma mark Post lighting depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Less -Writes");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLess);
        depthStencilDesc.depthWriteEnabled(false);
        
        m_dontWriteDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
    }
}

void SkyboxSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Sky");
    commandEncoder.setRenderPipelineState(m_skyboxPipelineState);
    commandEncoder.setDepthStencilState(m_dontWriteDepthStencilState);
    commandEncoder.setCullMode(MTL::CullModeFront);
    
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData"))
                                   , 0, BufferIndexFrameData);
    commandEncoder.setFragmentTexture(*std::any_cast<MTL::TexturePtr>(scene->background.sky.material->shaderData.getData("u_skybox")),
                                      TextureIndexBaseColor);
    
    for (auto &meshBuffer: scene->background.sky.mesh->vertexBuffers()) {
        commandEncoder.setVertexBuffer(meshBuffer.buffer(),
                                       meshBuffer.offset(),
                                       meshBuffer.argumentIndex());
    }
    
    
    for (auto &submesh: scene->background.sky.mesh->submeshes()) {
        commandEncoder.drawIndexedPrimitives(submesh.primitiveType(),
                                             submesh.indexCount(),
                                             submesh.indexType(),
                                             submesh.indexBuffer().buffer(),
                                             submesh.indexBuffer().offset());
    }
    commandEncoder.popDebugGroup();
}

}
