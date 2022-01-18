//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "compose_subpass.h"
#include "core/cpp_mtl_assert.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
ComposeSubpass::ComposeSubpass(MTL::RenderPassDescriptor* desc,
                               MTL::Device* device,
                               Scene* scene,
                               Camera* camera,
                               MTL::Library& shaderLibrary,
                               MTL::PixelFormat colorPixelFormat,
                               MTL::RenderPassDescriptor* gbufferDesc):
Subpass(desc, device, scene, camera),
_gbufferDesc(gbufferDesc) {
    CFErrorRef error = nullptr;
    
#pragma mark Directional lighting render pipeline setup
    {
        MTL::Function directionalVertexFunction = shaderLibrary.makeFunction("deferred_direction_lighting_vertex");
        MTL::Function directionalFragmentFunction = shaderLibrary.makeFunction("deferred_directional_lighting_fragment_traditional");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        renderPipelineDescriptor.label("Deferred Directional Lighting");
        renderPipelineDescriptor.vertexDescriptor(nullptr);
        renderPipelineDescriptor.vertexFunction(&directionalVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&directionalFragmentFunction);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(colorPixelFormat); // not set yet before draw
        renderPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(desc->stencilAttachment.texture().pixelFormat());
        
        _directionalLightPipelineState = _device->makeRenderPipelineState(renderPipelineDescriptor,
                                                                          &error);
        
        MTLAssert(error == nullptr, error,
                  "Failed to create directional light render pipeline state:");
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
        
        _directionLightDepthStencilState = device->makeDepthStencilState(depthStencilDesc);
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
        
        _quadVertexBuffer = device->makeBuffer(QuadVertices, sizeof(QuadVertices));
        _quadVertexBuffer.label("Quad Vertices");
    }
}

void ComposeSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Directional Light");
    commandEncoder.setFragmentTexture(_gbufferDesc->colorAttachments[RenderTargetAlbedo].texture(), RenderTargetAlbedo);
    commandEncoder.setFragmentTexture(_gbufferDesc->colorAttachments[RenderTargetNormal].texture(), RenderTargetNormal);
    commandEncoder.setFragmentTexture(_gbufferDesc->colorAttachments[RenderTargetDepth].texture(), RenderTargetDepth);
    
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setStencilReferenceValue(128);
    
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
