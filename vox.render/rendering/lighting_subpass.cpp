/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Implementation of the subpass class which performs Metal setup and per frame rendering for a
 traditional deferred subpass used for macOS devices without Apple Silicon and the
 iOS & tvOS simulators.
 */

#include "lighting_subpass.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

#include "core/CPPMetalAssert.hpp"

namespace vox {
LightingSubpass::LightingSubpass(MTL::View *view)
: Subpass(view) {
    loadMetal();
    loadScene();
}

LightingSubpass::~LightingSubpass() {
}

/// Create traditional deferred subpass specific Metal state objects
void LightingSubpass::loadMetal() {
    Subpass::loadMetal();
    
    CFErrorRef error = nullptr;
    
    
#pragma mark Point light render pipeline setup
    MTL::RenderPipelineDescriptor renderPipelineDescriptor;
    
    renderPipelineDescriptor.label("Light");
    renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view->colorPixelFormat());
    
    // Enable additive blending
    renderPipelineDescriptor.colorAttachments[RenderTargetLighting].blendingEnabled(true);
    renderPipelineDescriptor.colorAttachments[RenderTargetLighting].rgbBlendOperation(MTL::BlendOperationAdd);
    renderPipelineDescriptor.colorAttachments[RenderTargetLighting].alphaBlendOperation(MTL::BlendOperationAdd);
    renderPipelineDescriptor.colorAttachments[RenderTargetLighting].destinationRGBBlendFactor(MTL::BlendFactorOne);
    renderPipelineDescriptor.colorAttachments[RenderTargetLighting].destinationAlphaBlendFactor(MTL::BlendFactorOne);
    renderPipelineDescriptor.colorAttachments[RenderTargetLighting].sourceRGBBlendFactor(MTL::BlendFactorOne);
    renderPipelineDescriptor.colorAttachments[RenderTargetLighting].sourceAlphaBlendFactor(MTL::BlendFactorOne);
    
    renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
    renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
    
    MTL::Library shaderLibrary = makeShaderLibrary();
    
    MTL::Function lightVertexFunction = shaderLibrary.makeFunction("deferred_point_lighting_vertex");
    MTL::Function lightFragmentFunction = shaderLibrary.makeFunction("deferred_point_lighting_fragment_traditional");
    
    renderPipelineDescriptor.vertexFunction(&lightVertexFunction);
    renderPipelineDescriptor.fragmentFunction(&lightFragmentFunction);
    
    m_lightPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
    
    MTLAssert(error == nullptr, error, "Failed to create lighting render pipeline state");
}

/// Draw directional lighting, which, with a tradition deferred subpass needs to set GBuffers as
/// textures before executing common rendering code to draw the light
void LightingSubpass::drawDirectionalLight(MTL::RenderCommandEncoder &renderEncoder) {
    renderEncoder.pushDebugGroup("Draw Directional Light");
    renderEncoder.setFragmentTexture(m_albedo_specular_GBuffer, RenderTargetAlbedo);
    renderEncoder.setFragmentTexture(m_normal_shadow_GBuffer, RenderTargetNormal);
    renderEncoder.setFragmentTexture(m_depth_GBuffer, RenderTargetDepth);
    
    Subpass::drawDirectionalLightCommon(renderEncoder);
    
    renderEncoder.popDebugGroup();
}

/// Setup traditional deferred rendering specific pipeline and set GBuffer textures.  Then call
/// common subpass code to apply the point lights
void LightingSubpass::drawPointLights(MTL::RenderCommandEncoder &renderEncoder) {
    renderEncoder.pushDebugGroup("Draw Point Lights");
    
    renderEncoder.setRenderPipelineState(m_lightPipelineState);
    
    renderEncoder.setFragmentTexture(m_albedo_specular_GBuffer, RenderTargetAlbedo);
    renderEncoder.setFragmentTexture(m_normal_shadow_GBuffer, RenderTargetNormal);
    renderEncoder.setFragmentTexture(m_depth_GBuffer, RenderTargetDepth);
    
    // Call common base class method after setting state in the renderEncoder specific to the
    // traditional deferred subpass
    Subpass::drawPointLightsCommon(renderEncoder);
    
    renderEncoder.popDebugGroup();
}

}
