//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "point_light_subpass.h"
#include "core/cpp_mtl_assert.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
PointLightSubpass::PointLightSubpass(MTL::RenderPassDescriptor* desc,
                                     MTL::Device* device,
                                     Scene* scene,
                                     Camera* camera,
                                     MTL::Library& shaderLibrary,
                                     MTL::PixelFormat colorPixelFormat,
                                     MeshPtr icosahedronMesh,
                                     MTL::RenderPassDescriptor* gbufferDesc,
                                     const uint32_t numLights):
Subpass(desc, device, scene, camera),
_icosahedronMesh(icosahedronMesh),
_gbufferDesc(gbufferDesc),
_numLights(numLights) {
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
            
            _lightMaskPipelineState = _device->makeRenderPipelineState(renderPipelineDescriptor, &error);
            
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
            
            _lightMaskDepthStencilState = _device->makeDepthStencilState(depthStencilDesc);
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
        
        _pointLightDepthStencilState = _device->makeDepthStencilState(depthStencilDesc);
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
        
        _lightPipelineState = _device->makeRenderPipelineState(renderPipelineDescriptor, &error);
        
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
    commandEncoder.setRenderPipelineState(_lightMaskPipelineState);
    commandEncoder.setDepthStencilState(_lightMaskDepthStencilState);
    
    commandEncoder.setStencilReferenceValue(128);
    commandEncoder.setCullMode(MTL::CullModeFront);
    
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("lightsData")), 0, BufferIndexLightsData);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("lightPosition")), 0, BufferIndexLightsPosition);
    
    const std::vector<MeshBuffer> &vertexBuffers = _icosahedronMesh->vertexBuffers();
    commandEncoder.setVertexBuffer(vertexBuffers[0].buffer(), vertexBuffers[0].offset(), BufferIndexMeshPositions);
    
    const std::vector<Submesh> &icosahedronSubmesh = _icosahedronMesh->submeshes();
    
    commandEncoder.drawIndexedPrimitives(icosahedronSubmesh[0].primitiveType(),
                                         icosahedronSubmesh[0].indexCount(),
                                         icosahedronSubmesh[0].indexType(),
                                         icosahedronSubmesh[0].indexBuffer().buffer(),
                                         icosahedronSubmesh[0].indexBuffer().offset(),
                                         _numLights);
    
    commandEncoder.popDebugGroup();
#endif
}

void PointLightSubpass::drawPointLights(MTL::RenderCommandEncoder &commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Point Lights");
    
    commandEncoder.setRenderPipelineState(_lightPipelineState);
    
    commandEncoder.setFragmentTexture(_gbufferDesc->colorAttachments[RenderTargetAlbedo].texture(), RenderTargetAlbedo);
    commandEncoder.setFragmentTexture(_gbufferDesc->colorAttachments[RenderTargetNormal].texture(), RenderTargetNormal);
    commandEncoder.setFragmentTexture(_gbufferDesc->colorAttachments[RenderTargetDepth].texture(), RenderTargetDepth);
    
    commandEncoder.setDepthStencilState(_pointLightDepthStencilState);
    
    commandEncoder.setStencilReferenceValue(128);
    commandEncoder.setCullMode(MTL::CullModeBack);
    
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("lightsData")), 0, BufferIndexLightsData);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("lightPosition")), 0, BufferIndexLightsPosition);
    
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("lightsData")), 0, BufferIndexLightsData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("lightPosition")), 0, BufferIndexLightsPosition);
    
    const std::vector<MeshBuffer> &vertexBuffers = _icosahedronMesh->vertexBuffers();
    commandEncoder.setVertexBuffer(vertexBuffers[0].buffer(), vertexBuffers[0].offset(), BufferIndexMeshPositions);
    
    const std::vector<Submesh> &icosahedronSubmesh = _icosahedronMesh->submeshes();
    
    commandEncoder.drawIndexedPrimitives(icosahedronSubmesh[0].primitiveType(),
                                         icosahedronSubmesh[0].indexCount(),
                                         icosahedronSubmesh[0].indexType(),
                                         icosahedronSubmesh[0].indexBuffer().buffer(),
                                         icosahedronSubmesh[0].indexBuffer().offset(),
                                         _numLights);
    commandEncoder.popDebugGroup();
}

}
