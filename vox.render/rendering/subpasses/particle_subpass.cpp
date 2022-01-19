//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_subpass.h"
#include "rendering/render_pass.h"

#include "core/cpp_mtl_assert.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
ParticleSubpass::ParticleSubpass(View* view,
                                 Scene* scene,
                                 Camera* camera,
                                 MTL::Buffer &fairy,
                                 MTL::Texture &fairyMap,
                                 const uint32_t numLights,
                                 const uint32_t numFairyVertices):
Subpass(view, scene, camera),
_fairy(fairy),
_fairyMap(fairyMap),
_numLights(numLights),
_numFairyVertices(numFairyVertices) {
}

void ParticleSubpass::prepare() {
    CFErrorRef error = nullptr;

#pragma mark Fairy billboard render pipeline setup
    {
        MTL::Function fairyVertexFunction = _pass->library().makeFunction("fairy_vertex");
        MTL::Function fairyFragmentFunction = _pass->library().makeFunction("fairy_fragment");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        
        renderPipelineDescriptor.label("Fairy Drawing");
        renderPipelineDescriptor.vertexDescriptor(nullptr);
        renderPipelineDescriptor.vertexFunction(&fairyVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&fairyFragmentFunction);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(_view->colorPixelFormat());
        renderPipelineDescriptor.depthAttachmentPixelFormat(_view->depthStencilPixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(_view->depthStencilPixelFormat());
        renderPipelineDescriptor.colorAttachments[0].blendingEnabled(true);
        renderPipelineDescriptor.colorAttachments[0].rgbBlendOperation(MTL::BlendOperationAdd);
        renderPipelineDescriptor.colorAttachments[0].alphaBlendOperation(MTL::BlendOperationAdd);
        renderPipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
        renderPipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor(MTL::BlendFactorSourceAlpha);
        renderPipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor(MTL::BlendFactorOne);
        renderPipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor(MTL::BlendFactorOne);
        
        _fairyPipelineState = _view->device().makeRenderPipelineState(renderPipelineDescriptor);
        
        MTLAssert(error == nullptr, error, "Failed to create fairy render pipeline state: %@");
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

void ParticleSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Fairies");
    commandEncoder.setRenderPipelineState(_fairyPipelineState);
    commandEncoder.setDepthStencilState(_dontWriteDepthStencilState);
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setVertexBuffer(_fairy, 0, BufferIndexMeshPositions);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("lightsData")), 0, BufferIndexLightsData);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(_scene->shaderData.getData("lightPosition")), 0, BufferIndexLightsPosition);
    commandEncoder.setFragmentTexture(_fairyMap, TextureIndexAlpha);
    commandEncoder.drawPrimitives(MTL::PrimitiveTypeTriangleStrip, 0, _numFairyVertices, _numLights);
    commandEncoder.popDebugGroup();
}

}
