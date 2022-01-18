//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "skybox_subpass.h"
#include "core/cpp_mtl_assert.h"
#include "material/material.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(MTL::RenderPassDescriptor* desc,
                             MTL::Device* device,
                             Scene* scene,
                             Camera* camera,
                             MTL::Library& shaderLibrary,
                             MTL::PixelFormat colorPixelFormat):
Subpass(desc, device, scene, camera) {
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
        
        _skyboxPipelineState = _device->makeRenderPipelineState(renderPipelineDescriptor, &error);
        
        MTLAssert(error == nullptr, error, "Failed to create skybox render pipeline state: %@");
    }
    
#pragma mark Post lighting depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Less -Writes");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLess);
        depthStencilDesc.depthWriteEnabled(false);
        
        _dontWriteDepthStencilState = _device->makeDepthStencilState(depthStencilDesc);
    }
}

void SkyboxSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Sky");
    commandEncoder.setRenderPipelineState(_skyboxPipelineState);
    commandEncoder.setDepthStencilState(_dontWriteDepthStencilState);
    commandEncoder.setCullMode(MTL::CullModeFront);
    
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData"))
                                   , 0, BufferIndexFrameData);
    commandEncoder.setFragmentTexture(*std::any_cast<MTL::TexturePtr>(_scene->background.sky.material->shaderData.getData("u_skybox")),
                                      TextureIndexBaseColor);
    
    for (auto &meshBuffer: _scene->background.sky.mesh->vertexBuffers()) {
        commandEncoder.setVertexBuffer(meshBuffer.buffer(),
                                       meshBuffer.offset(),
                                       meshBuffer.argumentIndex());
    }
    
    
    for (auto &submesh: _scene->background.sky.mesh->submeshes()) {
        commandEncoder.drawIndexedPrimitives(submesh.primitiveType(),
                                             submesh.indexCount(),
                                             submesh.indexType(),
                                             submesh.indexBuffer().buffer(),
                                             submesh.indexBuffer().offset());
    }
    commandEncoder.popDebugGroup();
}

}
