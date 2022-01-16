//
//  compose_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "compose_subpass.h"
#include "core/CPPMetalAssert.hpp"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
ComposeSubpass::ComposeSubpass(MTL::RenderPassDescriptor* desc,
                               Scene* scene,
                               Camera* camera,
                               MTL::Library& shaderLibrary,
                               MTL::Device& m_device,
                               MTL::PixelFormat colorPixelFormat,
                               MTL::RenderPassDescriptor* gbuffer_desc):
Subpass(desc, m_device, scene, camera),
gbuffer_desc(gbuffer_desc) {
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
        
        m_directionalLightPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor,
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
        
        m_directionLightDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
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
        
        m_quadVertexBuffer = m_device.makeBuffer(QuadVertices, sizeof(QuadVertices));
        m_quadVertexBuffer.label("Quad Vertices");
    }
}

void ComposeSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Directional Light");
    commandEncoder.setFragmentTexture(gbuffer_desc->colorAttachments[RenderTargetAlbedo].texture(), RenderTargetAlbedo);
    commandEncoder.setFragmentTexture(gbuffer_desc->colorAttachments[RenderTargetNormal].texture(), RenderTargetNormal);
    commandEncoder.setFragmentTexture(gbuffer_desc->colorAttachments[RenderTargetDepth].texture(), RenderTargetDepth);
    
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setStencilReferenceValue(128);
    
    commandEncoder.setRenderPipelineState(m_directionalLightPipelineState);
    commandEncoder.setDepthStencilState(m_directionLightDepthStencilState);
    commandEncoder.setVertexBuffer(m_quadVertexBuffer, 0, BufferIndexMeshPositions);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    
    // Draw full screen quad
    commandEncoder.drawPrimitives(MTL::PrimitiveTypeTriangle, 0, 6);
    commandEncoder.popDebugGroup();
}

}
