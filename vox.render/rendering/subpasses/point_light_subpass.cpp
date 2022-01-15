//
//  point_light_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "point_light_subpass.h"
#include "core/CPPMetalAssert.hpp"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
PointLightSubpass::PointLightSubpass(MTL::RenderPassDescriptor* desc,
                                     Scene* scene,
                                     MTL::Library& shaderLibrary,
                                     MTL::Device& m_device,
                                     MTL::PixelFormat colorPixelFormat,
                                     Mesh& m_icosahedronMesh,
                                     MTL::RenderPassDescriptor* gbuffer_desc,
                                     const uint32_t NumLights):
Subpass(desc, scene),
m_icosahedronMesh(m_icosahedronMesh),
gbuffer_desc(gbuffer_desc),
NumLights(NumLights) {
    CFErrorRef error = nullptr;
    
#if LIGHT_STENCIL_CULLING
    // Setup objects for point light mask rendering
    {
#pragma mark Light mask render pipeline state setup
        {
            MTL::Function lightMaskVertex = shaderLibrary.makeFunction("light_mask_vertex");
            
            MTL::RenderPipelineDescriptor renderPipelineDescriptor;
            renderPipelineDescriptor.label("Point Light Mask");
            renderPipelineDescriptor.vertexDescriptor(nullptr);
            renderPipelineDescriptor.vertexFunction(&lightMaskVertex);
            renderPipelineDescriptor.fragmentFunction(nullptr);
            renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(colorPixelFormat);
            renderPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
            renderPipelineDescriptor.stencilAttachmentPixelFormat(desc->stencilAttachment.texture().pixelFormat());
            
            m_lightMaskPipelineState =
            m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
            
            MTLAssert(error == nullptr, error,
                      "Failed to create directional light mask pipeline state:");
        }
        
#pragma mark Light mask depth stencil state setup
        {
            MTL::StencilDescriptor stencilStateDesc;
            stencilStateDesc.stencilCompareFunction(MTL::CompareFunctionAlways);
            stencilStateDesc.stencilFailureOperation(MTL::StencilOperationKeep);
            stencilStateDesc.depthFailureOperation(MTL::StencilOperationIncrementClamp);
            stencilStateDesc.depthStencilPassOperation(MTL::StencilOperationKeep);
            stencilStateDesc.readMask(0x0);
            stencilStateDesc.writeMask(0xFF);
            MTL::DepthStencilDescriptor depthStencilDesc;
            depthStencilDesc.label("Point Light Mask");
            depthStencilDesc.depthWriteEnabled(false);
            depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLessEqual);
            depthStencilDesc.frontFaceStencil = stencilStateDesc;
            depthStencilDesc.backFaceStencil = stencilStateDesc;
            
            m_lightMaskDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
        }
    }
#endif // END LIGHT_STENCIL_CULLING
    
#pragma mark Point light depth state setup
    {
#if LIGHT_STENCIL_CULLING
        MTL::StencilDescriptor stencilStateDesc;
        stencilStateDesc.stencilCompareFunction(MTL::CompareFunctionLess);
        stencilStateDesc.stencilFailureOperation(MTL::StencilOperationKeep);
        stencilStateDesc.depthFailureOperation(MTL::StencilOperationKeep);
        stencilStateDesc.depthStencilPassOperation(MTL::StencilOperationKeep);
        stencilStateDesc.readMask(0xFF);
        stencilStateDesc.writeMask(0x0);
#else  // IF NOT LIGHT_STENCIL_CULLING
        MTL::StencilDescriptor stencilStateDesc;
#endif // END NOT LIGHT_STENCIL_CULLING
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.depthWriteEnabled(false);
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLessEqual);
        depthStencilDesc.frontFaceStencil = stencilStateDesc;
        depthStencilDesc.backFaceStencil = stencilStateDesc;
        depthStencilDesc.label("Point Light");
        
        m_pointLightDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
    }
    
#pragma mark Point light render pipeline setup
    {
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        
        renderPipelineDescriptor.label("Light");
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(colorPixelFormat);
        
        // Enable additive blending
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].blendingEnabled(true);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].rgbBlendOperation(MTL::BlendOperationAdd);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].alphaBlendOperation(MTL::BlendOperationAdd);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].destinationRGBBlendFactor(MTL::BlendFactorOne);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].destinationAlphaBlendFactor(MTL::BlendFactorOne);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].sourceRGBBlendFactor(MTL::BlendFactorOne);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].sourceAlphaBlendFactor(MTL::BlendFactorOne);
        
        renderPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(desc->stencilAttachment.texture().pixelFormat());
        
        MTL::Function lightVertexFunction = shaderLibrary.makeFunction("deferred_point_lighting_vertex");
        MTL::Function lightFragmentFunction = shaderLibrary.makeFunction("deferred_point_lighting_fragment_traditional");
        
        renderPipelineDescriptor.vertexFunction(&lightVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&lightFragmentFunction);
        
        m_lightPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
        
        MTLAssert(error == nullptr, error, "Failed to create lighting render pipeline state");
    }
}

void PointLightSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    drawPointLightMask(commandEncoder);
    drawPointLights(commandEncoder);
}

void PointLightSubpass::drawPointLightMask(MTL::RenderCommandEncoder &commandEncoder) {
#if LIGHT_STENCIL_CULLING
    commandEncoder.pushDebugGroup("Draw Light Mask");
    commandEncoder.setRenderPipelineState(m_lightMaskPipelineState);
    commandEncoder.setDepthStencilState(m_lightMaskDepthStencilState);
    
    commandEncoder.setStencilReferenceValue(128);
    commandEncoder.setCullMode(MTL::CullModeFront);
    
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("lightsData")), 0, BufferIndexLightsData);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("lightPosition")), 0, BufferIndexLightsPosition);
    
    const std::vector<MeshBuffer> &vertexBuffers = m_icosahedronMesh.vertexBuffers();
    commandEncoder.setVertexBuffer(vertexBuffers[0].buffer(), vertexBuffers[0].offset(), BufferIndexMeshPositions);
    
    const std::vector<Submesh> &icosahedronSubmesh = m_icosahedronMesh.submeshes();
    
    commandEncoder.drawIndexedPrimitives(icosahedronSubmesh[0].primitiveType(),
                                         icosahedronSubmesh[0].indexCount(),
                                         icosahedronSubmesh[0].indexType(),
                                         icosahedronSubmesh[0].indexBuffer().buffer(),
                                         icosahedronSubmesh[0].indexBuffer().offset(),
                                         NumLights);
    
    commandEncoder.popDebugGroup();
#endif
}

void PointLightSubpass::drawPointLights(MTL::RenderCommandEncoder &commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Point Lights");
    
    commandEncoder.setRenderPipelineState(m_lightPipelineState);
    
    commandEncoder.setFragmentTexture(gbuffer_desc->colorAttachments[RenderTargetAlbedo].texture(), RenderTargetAlbedo);
    commandEncoder.setFragmentTexture(gbuffer_desc->colorAttachments[RenderTargetNormal].texture(), RenderTargetNormal);
    commandEncoder.setFragmentTexture(gbuffer_desc->colorAttachments[RenderTargetDepth].texture(), RenderTargetDepth);
    
    commandEncoder.setDepthStencilState(m_pointLightDepthStencilState);
    
    commandEncoder.setStencilReferenceValue(128);
    commandEncoder.setCullMode(MTL::CullModeBack);
    
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("lightsData")), 0, BufferIndexLightsData);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("lightPosition")), 0, BufferIndexLightsPosition);
    
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("lightsData")), 0, BufferIndexLightsData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("lightPosition")), 0, BufferIndexLightsPosition);
    
    const std::vector<MeshBuffer> &vertexBuffers = m_icosahedronMesh.vertexBuffers();
    commandEncoder.setVertexBuffer(vertexBuffers[0].buffer(), vertexBuffers[0].offset(), BufferIndexMeshPositions);
    
    const std::vector<Submesh> &icosahedronSubmesh = m_icosahedronMesh.submeshes();
    
    commandEncoder.drawIndexedPrimitives(icosahedronSubmesh[0].primitiveType(),
                                         icosahedronSubmesh[0].indexCount(),
                                         icosahedronSubmesh[0].indexType(),
                                         icosahedronSubmesh[0].indexBuffer().buffer(),
                                         icosahedronSubmesh[0].indexBuffer().offset(),
                                         NumLights);
    commandEncoder.popDebugGroup();
}

}
