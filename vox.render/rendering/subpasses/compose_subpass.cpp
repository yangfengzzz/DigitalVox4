//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "compose_subpass.h"
#include "rendering/render_pass.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

#include <glog/logging.h>

namespace vox {
std::string ComposeSubpass::dependedPassName() {
    return "deferredPass";
}

ComposeSubpass::ComposeSubpass(View* view,
                               Scene* scene,
                               Camera* camera):
Subpass(view, scene, camera) {
}

void ComposeSubpass::setRenderPass(RenderPass* pass) {
    auto parentPass = pass->findPass(dependedPassName());
    if (parentPass) {
        Subpass::setRenderPass(pass);
        auto gbufferDesc = parentPass->renderPassDescriptor();
        _albedoTexture = &gbufferDesc->colorAttachments[RenderTargetAlbedo].texture();
        _normalTexture = &gbufferDesc->colorAttachments[RenderTargetNormal].texture();
        _depthTexture = &gbufferDesc->colorAttachments[RenderTargetDepth].texture();
    } else {
        LOG(ERROR) << "depend on deferredPass";
    }
}

void ComposeSubpass::prepare() {
#pragma mark Directional lighting render pipeline setup
    {
        MTL::Function directionalVertexFunction = _pass->library().makeFunction("deferred_direction_lighting_vertex");
        MTL::Function directionalFragmentFunction = _pass->library().makeFunction("deferred_directional_lighting_fragment_traditional");
        
        _directionalLightPipelineDescriptor.label("Deferred Directional Lighting");
        _directionalLightPipelineDescriptor.vertexDescriptor(nullptr);
        _directionalLightPipelineDescriptor.vertexFunction(&directionalVertexFunction);
        _directionalLightPipelineDescriptor.fragmentFunction(&directionalFragmentFunction);
        _directionalLightPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(_view->colorPixelFormat());
        _directionalLightPipelineDescriptor.depthAttachmentPixelFormat(_view->depthStencilPixelFormat());
        _directionalLightPipelineDescriptor.stencilAttachmentPixelFormat(_view->depthStencilPixelFormat());
    }
    
#pragma mark Directional lighting mask depth stencil state setup
    {
#if LIGHT_STENCIL_CULLING
        // Stencil state setup so direction lighting fragment shader only executed on pixels
        // drawn in GBuffer stage (i.e. mask out the background/sky)
        MTL::StencilDescriptor stencilStateDesc;
        stencilStateDesc.stencilCompareFunction(MTL::CompareFunctionEqual);
        stencilStateDesc.stencilFailureOperation(MTL::StencilOperationKeep);
        stencilStateDesc.depthFailureOperation(MTL::StencilOperationKeep);
        stencilStateDesc.depthStencilPassOperation(MTL::StencilOperationKeep);
        stencilStateDesc.readMask(0xFF);
        stencilStateDesc.writeMask(0x0);
#else
        MTL::StencilDescriptor stencilStateDesc;
#endif
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Deferred Directional Lighting");
        depthStencilDesc.depthWriteEnabled(false);
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionAlways);
        depthStencilDesc.frontFaceStencil = stencilStateDesc;
        depthStencilDesc.backFaceStencil = stencilStateDesc;
        
        _directionLightDepthStencilState = _view->device().makeDepthStencilState(depthStencilDesc);
    }
    
#pragma mark Create quad for fullscreen composition drawing
    {
        static const SimpleVertex QuadVertices[] =
        {
            {{-1.0f, -1.0f,}},
            {{-1.0f, 1.0f,}},
            {{1.0f, -1.0f,}},
            
            {{1.0f, -1.0f,}},
            {{-1.0f, 1.0f,}},
            {{1.0f, 1.0f,}},
        };
        
        _quadVertexBuffer = _view->device().makeBuffer(QuadVertices, sizeof(QuadVertices));
        _quadVertexBuffer.label("Quad Vertices");
    }
}

void ComposeSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Directional Light");
    commandEncoder.setFragmentTexture(*_albedoTexture, RenderTargetAlbedo);
    commandEncoder.setFragmentTexture(*_normalTexture, RenderTargetNormal);
    commandEncoder.setFragmentTexture(*_depthTexture, RenderTargetDepth);
    
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setStencilReferenceValue(128);
    
    auto _directionalLightPipelineState = _view->device().resourceCache().requestRenderPipelineState(_directionalLightPipelineDescriptor);
    commandEncoder.setRenderPipelineState(_directionalLightPipelineState);
    commandEncoder.setDepthStencilState(_directionLightDepthStencilState);
    commandEncoder.setVertexBuffer(_quadVertexBuffer, 0, BufferIndexMeshPositions);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    
    // Draw full screen quad
    commandEncoder.drawPrimitives(MTL::PrimitiveTypeTriangle, 0, 6);
    commandEncoder.popDebugGroup();
}

}
