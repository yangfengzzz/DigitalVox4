//
//  shadow_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "shadow_subpass.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
ShadowSubpass::ShadowSubpass(MTL::RenderPassDescriptor* desc,
                             sg::Scene* scene,
                             MTL::Library& shaderLibrary,
                             MTL::Device& m_device,
                             std::vector<Mesh> *m_meshes):
Subpass(desc, scene),
m_meshes(m_meshes) {
    MTL::PixelFormat shadowMapPixelFormat = MTL::PixelFormatDepth16Unorm;
    CFErrorRef error = nullptr;

#pragma mark Shadow pass render pipeline setup
    {
        MTL::Function *shadowVertexFunction = shaderLibrary.newFunctionWithName("shadow_vertex");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        renderPipelineDescriptor.label("Shadow Gen");
        renderPipelineDescriptor.vertexDescriptor(nullptr);
        renderPipelineDescriptor.vertexFunction(shadowVertexFunction);
        renderPipelineDescriptor.fragmentFunction(nullptr);
        renderPipelineDescriptor.depthAttachmentPixelFormat(shadowMapPixelFormat);
        
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
    
#pragma mark Shadow map setup
    {
        MTL::TextureDescriptor shadowTextureDesc;
        
        shadowTextureDesc.pixelFormat(shadowMapPixelFormat);
        shadowTextureDesc.width(2048);
        shadowTextureDesc.height(2048);
        shadowTextureDesc.mipmapLevelCount(1);
        shadowTextureDesc.resourceOptions(MTL::ResourceStorageModePrivate);
        shadowTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        
        m_shadowMap = m_device.makeTexture(shadowTextureDesc);
        m_shadowMap.label("Shadow Map");
    }
}

void ShadowSubpass::draw(MTL::RenderCommandEncoder commandEncoder) {
    commandEncoder.label("Shadow Map Pass");
    
    commandEncoder.setRenderPipelineState(m_shadowGenPipelineState);
    commandEncoder.setDepthStencilState(m_shadowDepthStencilState);
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setDepthBias(0.015, 7, 0.02);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    
    drawMeshes(commandEncoder);
}

void ShadowSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    for (auto &mesh: *m_meshes) {
        for (auto &meshBuffer: mesh.vertexBuffers()) {
            renderEncoder.setVertexBuffer(meshBuffer.buffer(),
                                          meshBuffer.offset(),
                                          meshBuffer.argumentIndex());
        }
        
        for (auto &submesh: mesh.submeshes()) {
            // Set any textures read/sampled from the render pipeline
            const std::vector<MTL::Texture> &submeshTextures = submesh.textures();
            
            renderEncoder.setFragmentTexture(submeshTextures[TextureIndexBaseColor], TextureIndexBaseColor);
            
            renderEncoder.setFragmentTexture(submeshTextures[TextureIndexNormal], TextureIndexNormal);
            
            renderEncoder.setFragmentTexture(submeshTextures[TextureIndexSpecular], TextureIndexSpecular);
            
            renderEncoder.drawIndexedPrimitives(submesh.primitiveType(),
                                                submesh.indexCount(),
                                                submesh.indexType(),
                                                submesh.indexBuffer().buffer(),
                                                submesh.indexBuffer().offset());
        }
    }
}

}
