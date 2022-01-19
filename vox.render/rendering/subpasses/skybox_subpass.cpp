//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "skybox_subpass.h"
#include "rendering/render_pass.h"

#include "material/material.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(View* view,
                             Scene* scene,
                             Camera* camera):
Subpass(view, scene, camera) {
}

void SkyboxSubpass::prepare() {
#pragma mark Sky render pipeline setup
    {
        MTL::Function skyboxVertexFunction = _pass->library().makeFunction("skybox_vertex");
        MTL::Function skyboxFragmentFunction = _pass->library().makeFunction("skybox_fragment");
        
        _skyboxPipelineDescriptor.label("Sky");
        _skyboxPipelineDescriptor.vertexDescriptor(&(_scene->background.sky.mesh->vertexDescriptor()));
        _skyboxPipelineDescriptor.vertexFunction(&skyboxVertexFunction);
        _skyboxPipelineDescriptor.fragmentFunction(&skyboxFragmentFunction);
        _skyboxPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(_view->colorPixelFormat());
        _skyboxPipelineDescriptor.depthAttachmentPixelFormat(_view->depthStencilPixelFormat());
        _skyboxPipelineDescriptor.stencilAttachmentPixelFormat(_view->depthStencilPixelFormat());
    }
    
#pragma mark Post lighting depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Less -Writes");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLess);
        depthStencilDesc.depthWriteEnabled(false);
        
        _dontWriteDepthStencilState = _view->device().makeDepthStencilState(depthStencilDesc);
    }
}

void SkyboxSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Sky");
    auto _skyboxPipelineState = _view->device().resourceCache().requestRenderPipelineState(_skyboxPipelineDescriptor);
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
