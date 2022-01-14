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
    m_singlePassDeferred = false;
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
    {
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
    
#pragma mark GBuffer render pass descriptor setup
    // Create a render pass descriptor to create an encoder for rendering to the GBuffers.
    // The encoder stores rendered data of each attachment when encoding ends.
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetLighting].loadAction(MTL::LoadActionDontCare);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetLighting].storeAction(MTL::StoreActionDontCare);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].loadAction(MTL::LoadActionDontCare);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].storeAction(MTL::StoreActionStore);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].loadAction(MTL::LoadActionDontCare);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].storeAction(MTL::StoreActionStore);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].loadAction(MTL::LoadActionDontCare);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].storeAction(MTL::StoreActionStore);
    m_GBufferRenderPassDescriptor.depthAttachment.clearDepth(1.0);
    m_GBufferRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
    m_GBufferRenderPassDescriptor.depthAttachment.storeAction(MTL::StoreActionStore);
    
    m_GBufferRenderPassDescriptor.stencilAttachment.clearStencil(0);
    m_GBufferRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionClear);
    m_GBufferRenderPassDescriptor.stencilAttachment.storeAction(MTL::StoreActionStore);
    
    // Create a render pass descriptor for thelighting and composition pass
    
    // Whatever rendered in the final pass needs to be stored so it can be displayed
    m_finalRenderPassDescriptor.colorAttachments[0].storeAction(MTL::StoreActionStore);
    m_finalRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionLoad);
    m_finalRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionLoad);
}

/// Respond to view size change
void LightingSubpass::drawableSizeWillChange(MTL::View *view, const MTL::Size &size) {
    // The subpass base class allocates all GBuffers >except< lighting GBuffer (since with the
    // single-pass deferred subpass the lighting buffer is the same as the drawable)
    Subpass::drawableSizeWillChange(size, MTL::StorageModePrivate);
    
    // Re-set GBuffer textures in the GBuffer render pass descriptor after they have been
    // reallocated by a resize
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].texture(m_albedo_specular_GBuffer);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].texture(m_normal_shadow_GBuffer);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].texture(m_depth_GBuffer);
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

/// Frame drawing routine
void LightingSubpass::drawInView(MTL::View *view) {
    {
        MTL::CommandBuffer commandBuffer = Subpass::beginFrame();
        commandBuffer.label("Shadow & GBuffer Commands");
        
        Subpass::drawShadow(commandBuffer);
        
        m_GBufferRenderPassDescriptor.depthAttachment.texture(*view->depthStencilTexture());
        m_GBufferRenderPassDescriptor.stencilAttachment.texture(*view->depthStencilTexture());
        
        MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor(m_GBufferRenderPassDescriptor);
        renderEncoder.label("GBuffer Generation");
        
        Subpass::drawGBuffer(renderEncoder);
        
        renderEncoder.endEncoding();
        
        // Commit commands so that Metal can begin working on non-drawable dependant work without
        // waiting for a drawable to become avaliable
        commandBuffer.commit();
    }
    
    {
        MTL::CommandBuffer commandBuffer = Subpass::beginDrawableCommands();
        
        commandBuffer.label("Lighting Commands");
        
        MTL::Texture *drawableTexture = Subpass::currentDrawableTexture();
        
        // The final pass can only render if a drawable is available, otherwise it needs to skip
        // rendering this frame.
        if (drawableTexture) {
            // Render the lighting and composition pass
            
            m_finalRenderPassDescriptor.colorAttachments[0].texture(*drawableTexture);
            m_finalRenderPassDescriptor.depthAttachment.texture(*m_view->depthStencilTexture());
            m_finalRenderPassDescriptor.stencilAttachment.texture(*m_view->depthStencilTexture());
            
            MTL::RenderCommandEncoder renderEncoder =
            commandBuffer.renderCommandEncoderWithDescriptor(m_finalRenderPassDescriptor);
            renderEncoder.label("Lighting & Composition Pass");
            
            drawDirectionalLight(renderEncoder);
            
            Subpass::drawPointLightMask(renderEncoder);
            
            drawPointLights(renderEncoder);
            
            Subpass::drawSky(renderEncoder);
            
            Subpass::drawFairies(renderEncoder);
            
            renderEncoder.endEncoding();
        }
        
        Subpass::endFrame(commandBuffer);
    }
}

}
