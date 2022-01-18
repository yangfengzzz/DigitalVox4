//
//  particle_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "particle_subpass.h"
#include "core/cpp_mtl_Assert.hpp"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
ParticleSubpass::ParticleSubpass(MTL::RenderPassDescriptor* desc,
                                 Scene* scene,
                                 Camera* camera,
                                 MTL::Library& shaderLibrary,
                                 MTL::Device& m_device,
                                 MTL::PixelFormat colorPixelFormat,
                                 MTL::Buffer &m_fairy,
                                 MTL::Texture &m_fairyMap,
                                 const uint32_t NumLights,
                                 const uint32_t NumFairyVertices):
Subpass(desc, m_device, scene, camera),
m_fairy(m_fairy),
m_fairyMap(m_fairyMap),
NumLights(NumLights),
NumFairyVertices(NumFairyVertices) {
    CFErrorRef error = nullptr;

#pragma mark Fairy billboard render pipeline setup
    {
        MTL::Function fairyVertexFunction = shaderLibrary.makeFunction("fairy_vertex");
        MTL::Function fairyFragmentFunction = shaderLibrary.makeFunction("fairy_fragment");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        
        renderPipelineDescriptor.label("Fairy Drawing");
        renderPipelineDescriptor.vertexDescriptor(nullptr);
        renderPipelineDescriptor.vertexFunction(&fairyVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&fairyFragmentFunction);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(colorPixelFormat);
        renderPipelineDescriptor.depthAttachmentPixelFormat(desc->depthAttachment.texture().pixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(desc->stencilAttachment.texture().pixelFormat());
        renderPipelineDescriptor.colorAttachments[0].blendingEnabled(true);
        renderPipelineDescriptor.colorAttachments[0].rgbBlendOperation(MTL::BlendOperationAdd);
        renderPipelineDescriptor.colorAttachments[0].alphaBlendOperation(MTL::BlendOperationAdd);
        renderPipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
        renderPipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor(MTL::BlendFactorSourceAlpha);
        renderPipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor(MTL::BlendFactorOne);
        renderPipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor(MTL::BlendFactorOne);
        
        m_fairyPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor);
        
        MTLAssert(error == nullptr, error, "Failed to create fairy render pipeline state: %@");
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

void ParticleSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Fairies");
    commandEncoder.setRenderPipelineState(m_fairyPipelineState);
    commandEncoder.setDepthStencilState(m_dontWriteDepthStencilState);
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setVertexBuffer(m_fairy, 0, BufferIndexMeshPositions);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("lightsData")), 0, BufferIndexLightsData);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("lightPosition")), 0, BufferIndexLightsPosition);
    commandEncoder.setFragmentTexture(m_fairyMap, TextureIndexAlpha);
    commandEncoder.drawPrimitives(MTL::PrimitiveTypeTriangleStrip, 0, NumFairyVertices, NumLights);
    commandEncoder.popDebugGroup();
}

}
