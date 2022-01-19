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
#pragma mark Fairy billboard render pipeline setup
    {
        MTL::Function fairyVertexFunction = _pass->library().makeFunction("fairy_vertex");
        MTL::Function fairyFragmentFunction = _pass->library().makeFunction("fairy_fragment");
                
        _fairyPipelineDescriptor.label("Fairy Drawing");
        _fairyPipelineDescriptor.vertexDescriptor(nullptr);
        _fairyPipelineDescriptor.vertexFunction(&fairyVertexFunction);
        _fairyPipelineDescriptor.fragmentFunction(&fairyFragmentFunction);
        _fairyPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(_view->colorPixelFormat());
        _fairyPipelineDescriptor.depthAttachmentPixelFormat(_view->depthStencilPixelFormat());
        _fairyPipelineDescriptor.stencilAttachmentPixelFormat(_view->depthStencilPixelFormat());
        _fairyPipelineDescriptor.colorAttachments[0].blendingEnabled(true);
        _fairyPipelineDescriptor.colorAttachments[0].rgbBlendOperation(MTL::BlendOperationAdd);
        _fairyPipelineDescriptor.colorAttachments[0].alphaBlendOperation(MTL::BlendOperationAdd);
        _fairyPipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
        _fairyPipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor(MTL::BlendFactorSourceAlpha);
        _fairyPipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor(MTL::BlendFactorOne);
        _fairyPipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor(MTL::BlendFactorOne);
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
    auto _fairyPipelineState = _view->device().resourceCache().requestRenderPipelineState(_fairyPipelineDescriptor);
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
