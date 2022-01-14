/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Implementation of subpass class which performs Metal setup and per frame rendering
 */

#include "lighting_subpass.h"

#include <sys/sysctl.h>
#include <simd/simd.h>
#include <stdlib.h>
#include <array>
#include <string>

#include "material/texture_loader.h"
#include "graphics/mesh_loader.h"
#include "graphics/primitive_mesh.h"
#include "math_utilities.h"
#include "core/CPPMetalAssert.hpp"

using namespace simd;

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
// Number of vertices in our 2D fairy model
static const uint32_t NumFairyVertices = 7;

LightingSubpass::LightingSubpass(MTL::View* view)
: m_view(view),
m_device(view->device())
{
}

LightingSubpass::~LightingSubpass() {
}

/// Create Metal render state objects
void LightingSubpass::loadMetal(MTL::VertexDescriptor& m_defaultVertexDescriptor,
                                MTL::VertexDescriptor& m_skyVertexDescriptor) {
    // Create and load the basic Metal state objects
    CFErrorRef error = nullptr;
    
    printf("Selected Device: %s\n", m_view->device().name());
    
    MTL::Library shaderLibrary = makeShaderLibrary();
    
    m_view->depthStencilPixelFormat(MTL::PixelFormatDepth32Float_Stencil8);
    m_view->colorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
    
    m_albedo_specular_GBufferFormat = MTL::PixelFormatRGBA8Unorm_sRGB;
    m_normal_shadow_GBufferFormat = MTL::PixelFormatRGBA8Snorm;
    m_depth_GBufferFormat = MTL::PixelFormatR32Float;
    
#pragma mark GBuffer render pipeline setup
    {
        {
            MTL::Function GBufferVertexFunction = shaderLibrary.makeFunction("gbuffer_vertex");
            MTL::Function GBufferFragmentFunction = shaderLibrary.makeFunction("gbuffer_fragment");
            
            MTL::RenderPipelineDescriptor renderPipelineDescriptor;
            
            renderPipelineDescriptor.label("G-buffer Creation");
            renderPipelineDescriptor.vertexDescriptor(&m_defaultVertexDescriptor);
            
            renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(MTL::PixelFormatInvalid);
            renderPipelineDescriptor.colorAttachments[RenderTargetAlbedo].pixelFormat(m_albedo_specular_GBufferFormat);
            renderPipelineDescriptor.colorAttachments[RenderTargetNormal].pixelFormat(m_normal_shadow_GBufferFormat);
            renderPipelineDescriptor.colorAttachments[RenderTargetDepth].pixelFormat(m_depth_GBufferFormat);
            renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            
            renderPipelineDescriptor.vertexFunction(&GBufferVertexFunction);
            renderPipelineDescriptor.fragmentFunction(&GBufferFragmentFunction);
            
            m_GBufferPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
            
            MTLAssert(error == nullptr, error, "Failed to create GBuffer render pipeline state");
        }
        
#pragma mark GBuffer depth state setup
        {
#if LIGHT_STENCIL_CULLING
            MTL::StencilDescriptor stencilStateDesc;
            stencilStateDesc.stencilCompareFunction(MTL::CompareFunctionAlways);
            stencilStateDesc.stencilFailureOperation(MTL::StencilOperationKeep);
            stencilStateDesc.depthFailureOperation(MTL::StencilOperationKeep);
            stencilStateDesc.depthStencilPassOperation(MTL::StencilOperationReplace);
            stencilStateDesc.readMask(0x0);
            stencilStateDesc.writeMask(0xFF);
#else
            MTL::StencilDescriptor stencilStateDesc;
#endif
            MTL::DepthStencilDescriptor depthStencilDesc;
            depthStencilDesc.label("G-buffer Creation");
            depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLess);
            depthStencilDesc.depthWriteEnabled(true);
            depthStencilDesc.frontFaceStencil = stencilStateDesc;
            depthStencilDesc.backFaceStencil = stencilStateDesc;
            
            m_GBufferDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
        }
    }
    
    // Setup render state to apply directional light and shadow in final pass
    {
#pragma mark Directional lighting render pipeline setup
        {
            MTL::Function directionalVertexFunction = shaderLibrary.makeFunction("deferred_direction_lighting_vertex");
            MTL::Function directionalFragmentFunction = shaderLibrary.makeFunction("deferred_directional_lighting_fragment_traditional");
            
            MTL::RenderPipelineDescriptor renderPipelineDescriptor;
            renderPipelineDescriptor.label("Deferred Directional Lighting");
            renderPipelineDescriptor.vertexDescriptor(nullptr);
            renderPipelineDescriptor.vertexFunction(&directionalVertexFunction);
            renderPipelineDescriptor.fragmentFunction(&directionalFragmentFunction);
            renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view->colorPixelFormat());
            renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            
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
    }
    
#pragma mark Fairy billboard render pipeline setup
    {
        MTL::Function fairyVertexFunction = shaderLibrary.makeFunction("fairy_vertex");
        MTL::Function fairyFragmentFunction = shaderLibrary.makeFunction("fairy_fragment");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        
        renderPipelineDescriptor.label("Fairy Drawing");
        renderPipelineDescriptor.vertexDescriptor(nullptr);
        renderPipelineDescriptor.vertexFunction(&fairyVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&fairyFragmentFunction);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view->colorPixelFormat());
        renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
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
    
#pragma mark Sky render pipeline setup
    {
        MTL::Function skyboxVertexFunction = shaderLibrary.makeFunction("skybox_vertex");
        MTL::Function skyboxFragmentFunction = shaderLibrary.makeFunction("skybox_fragment");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        renderPipelineDescriptor.label("Sky");
        renderPipelineDescriptor.vertexDescriptor(&m_skyVertexDescriptor);
        renderPipelineDescriptor.vertexFunction(&skyboxVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&skyboxFragmentFunction);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view->colorPixelFormat());
        renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
        
        m_skyboxPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
        
        MTLAssert(error == nullptr, error, "Failed to create skybox render pipeline state: %@");
    }
    
#pragma mark Post lighting depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Less -Writes");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLess);
        depthStencilDesc.depthWriteEnabled(false);
        
        m_dontWriteDepthStencilState = m_device.newDepthStencilStateWithDescriptor(depthStencilDesc);
    }
    
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
            renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view->colorPixelFormat());
            renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            
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
        
        MTL::Function lightVertexFunction = shaderLibrary.makeFunction("deferred_point_lighting_vertex");
        MTL::Function lightFragmentFunction = shaderLibrary.makeFunction("deferred_point_lighting_fragment_traditional");
        
        renderPipelineDescriptor.vertexFunction(&lightVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&lightFragmentFunction);
        
        m_lightPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
        
        MTLAssert(error == nullptr, error, "Failed to create lighting render pipeline state");
    }
}

/// Called whenever view changes orientation or layout is changed
void LightingSubpass::drawableSizeWillChange(MTL::Size size, MTL::StorageMode GBufferStorageMode) {    
    MTL::TextureDescriptor GBufferTextureDesc;
    
    GBufferTextureDesc.pixelFormat(MTL::PixelFormatRGBA8Unorm_sRGB);
    GBufferTextureDesc.width(size.width);
    GBufferTextureDesc.height(size.height);
    GBufferTextureDesc.mipmapLevelCount(1);
    GBufferTextureDesc.textureType(MTL::TextureType2D);
    
    if (GBufferStorageMode == MTL::StorageModePrivate) {
        GBufferTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    } else {
        GBufferTextureDesc.usage(MTL::TextureUsageRenderTarget);
    }
    
    GBufferTextureDesc.storageMode(GBufferStorageMode);
    
    GBufferTextureDesc.pixelFormat(m_albedo_specular_GBufferFormat);
    m_albedo_specular_GBuffer = m_device.makeTexture(GBufferTextureDesc);
    
    GBufferTextureDesc.pixelFormat(m_normal_shadow_GBufferFormat);
    m_normal_shadow_GBuffer = m_device.makeTexture(GBufferTextureDesc);
    
    GBufferTextureDesc.pixelFormat(m_depth_GBufferFormat);
    m_depth_GBuffer = m_device.makeTexture(GBufferTextureDesc);
    
    m_albedo_specular_GBuffer.label("Albedo + Shadow GBuffer");
    m_normal_shadow_GBuffer.label("Normal + Specular GBuffer");
    m_depth_GBuffer.label("Depth GBuffer");
}

#pragma mark Common Rendering Code

/// Draw the Mesh objects with the given renderEncoder
void LightingSubpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder, std::vector<Mesh> *m_meshes) {
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

/// Perform cleanup operations including presenting the drawable and committing the command buffer
/// for the current frame.  Also, when enabled, draw buffer examination elements before all this.
void LightingSubpass::endFrame(MTL::CommandBuffer &commandBuffer) {
    // Schedule a present once the framebuffer is complete using the current drawable
    if (m_view->currentDrawable()) {
        // Create a scheduled handler functor for Metal to present the drawable when the command
        // buffer has been scheduled by the kernel.
        struct PresentationScheduledHandler : public MTL::CommandBufferHandler {
            MTL::Drawable m_drawable;
            
            PresentationScheduledHandler(MTL::Drawable drawable)
            : m_drawable(drawable) {
            }
            
            void operator()(const MTL::CommandBuffer &) {
                m_drawable.present();
                delete this;
            }
        };
        
        PresentationScheduledHandler *scheduledHandler =
        new PresentationScheduledHandler(*m_view->currentDrawable());
        
        commandBuffer.addScheduledHandler(*scheduledHandler);
    }
    
    // Finalize rendering here & push the command buffer to the GPU
    commandBuffer.commit();
}

/// Draw to the three textures which compose the GBuffer
void LightingSubpass::drawGBuffer(MTL::RenderCommandEncoder &renderEncoder,
                                  std::vector<Mesh> *m_meshes,
                                  MTL::Buffer& m_uniformBuffer,
                                  MTL::Texture& m_shadowMap) {
    renderEncoder.pushDebugGroup("Draw G-Buffer");
    renderEncoder.setCullMode(MTL::CullModeBack);
    renderEncoder.setRenderPipelineState(m_GBufferPipelineState);
    renderEncoder.setDepthStencilState(m_GBufferDepthStencilState);
    renderEncoder.setStencilReferenceValue(128);
    renderEncoder.setVertexBuffer(m_uniformBuffer, 0, BufferIndexFrameData);
    renderEncoder.setFragmentBuffer(m_uniformBuffer, 0, BufferIndexFrameData);
    renderEncoder.setFragmentTexture(m_shadowMap, TextureIndexShadow);
    
    drawMeshes(renderEncoder, m_meshes);
    renderEncoder.popDebugGroup();
}

/// Draw the directional ("sun") light in deferred pass.  Use stencil buffer to limit execution
/// of the shader to only those pixels that should be lit
void LightingSubpass::drawDirectionalLight(MTL::RenderCommandEncoder &renderEncoder, MTL::Buffer& m_quadVertexBuffer, MTL::Buffer& m_uniformBuffer) {
    renderEncoder.pushDebugGroup("Draw Directional Light");
    renderEncoder.setFragmentTexture(m_albedo_specular_GBuffer, RenderTargetAlbedo);
    renderEncoder.setFragmentTexture(m_normal_shadow_GBuffer, RenderTargetNormal);
    renderEncoder.setFragmentTexture(m_depth_GBuffer, RenderTargetDepth);
    
    renderEncoder.setCullMode(MTL::CullModeBack);
    renderEncoder.setStencilReferenceValue(128);
    
    renderEncoder.setRenderPipelineState(m_directionalLightPipelineState);
    renderEncoder.setDepthStencilState(m_directionLightDepthStencilState);
    renderEncoder.setVertexBuffer(m_quadVertexBuffer, 0, BufferIndexMeshPositions);
    renderEncoder.setVertexBuffer(m_uniformBuffer, 0, BufferIndexFrameData);
    renderEncoder.setFragmentBuffer(m_uniformBuffer, 0, BufferIndexFrameData);
    
    // Draw full screen quad
    renderEncoder.drawPrimitives(MTL::PrimitiveTypeTriangle, 0, 6);
    renderEncoder.popDebugGroup();
}

/// Render to stencil buffer only to increment stencil on that fragments in front
/// of the backside of each light volume
void LightingSubpass::drawPointLightMask(MTL::RenderCommandEncoder &renderEncoder,
                                         MTL::Buffer& m_lightsData,
                                         MTL::Buffer& m_lightPosition,
                                         MTL::Buffer& m_uniformBuffer,
                                         Mesh& m_icosahedronMesh) {
#if LIGHT_STENCIL_CULLING
    renderEncoder.pushDebugGroup("Draw Light Mask");
    renderEncoder.setRenderPipelineState(m_lightMaskPipelineState);
    renderEncoder.setDepthStencilState(m_lightMaskDepthStencilState);
    
    renderEncoder.setStencilReferenceValue(128);
    renderEncoder.setCullMode(MTL::CullModeFront);
    
    renderEncoder.setVertexBuffer(m_uniformBuffer, 0, BufferIndexFrameData);
    renderEncoder.setFragmentBuffer(m_uniformBuffer, 0, BufferIndexFrameData);
    renderEncoder.setVertexBuffer(m_lightsData, 0, BufferIndexLightsData);
    renderEncoder.setVertexBuffer(m_lightPosition, 0, BufferIndexLightsPosition);
    
    const std::vector<MeshBuffer> &vertexBuffers = m_icosahedronMesh.vertexBuffers();
    renderEncoder.setVertexBuffer(vertexBuffers[0].buffer(), vertexBuffers[0].offset(), BufferIndexMeshPositions);
    
    const std::vector<Submesh> &icosahedronSubmesh = m_icosahedronMesh.submeshes();
    
    renderEncoder.drawIndexedPrimitives(icosahedronSubmesh[0].primitiveType(),
                                        icosahedronSubmesh[0].indexCount(),
                                        icosahedronSubmesh[0].indexType(),
                                        icosahedronSubmesh[0].indexBuffer().buffer(),
                                        icosahedronSubmesh[0].indexBuffer().offset(),
                                        NumLights);
    
    renderEncoder.popDebugGroup();
#endif
}

/// Performs operations common to both single-pass and traditional deferred renders for drawing point lights.
/// Called by derived subpass classes  after they have set up any subpass specific specific state
/// (such as setting GBuffer textures with the traditional deferred subpass not needed for the single-pass subpass)
void LightingSubpass::drawPointLights(MTL::RenderCommandEncoder &renderEncoder,
                                      MTL::Buffer& m_lightsData,
                                      MTL::Buffer& m_lightPosition,
                                      MTL::Buffer& m_uniformBuffer,
                                      Mesh& m_icosahedronMesh) {
    renderEncoder.pushDebugGroup("Draw Point Lights");
    
    renderEncoder.setRenderPipelineState(m_lightPipelineState);
    
    renderEncoder.setFragmentTexture(m_albedo_specular_GBuffer, RenderTargetAlbedo);
    renderEncoder.setFragmentTexture(m_normal_shadow_GBuffer, RenderTargetNormal);
    renderEncoder.setFragmentTexture(m_depth_GBuffer, RenderTargetDepth);
    
    renderEncoder.setDepthStencilState(m_pointLightDepthStencilState);
    
    renderEncoder.setStencilReferenceValue(128);
    renderEncoder.setCullMode(MTL::CullModeBack);
    
    renderEncoder.setVertexBuffer(m_uniformBuffer, 0, BufferIndexFrameData);
    renderEncoder.setVertexBuffer(m_lightsData, 0, BufferIndexLightsData);
    renderEncoder.setVertexBuffer(m_lightPosition, 0, BufferIndexLightsPosition);
    
    renderEncoder.setFragmentBuffer(m_uniformBuffer, 0, BufferIndexFrameData);
    renderEncoder.setFragmentBuffer(m_lightsData, 0, BufferIndexLightsData);
    renderEncoder.setFragmentBuffer(m_lightPosition, 0, BufferIndexLightsPosition);
    
    const std::vector<MeshBuffer> &vertexBuffers = m_icosahedronMesh.vertexBuffers();
    renderEncoder.setVertexBuffer(vertexBuffers[0].buffer(), vertexBuffers[0].offset(), BufferIndexMeshPositions);
    
    const std::vector<Submesh> &icosahedronSubmesh = m_icosahedronMesh.submeshes();
    
    renderEncoder.drawIndexedPrimitives(icosahedronSubmesh[0].primitiveType(),
                                        icosahedronSubmesh[0].indexCount(),
                                        icosahedronSubmesh[0].indexType(),
                                        icosahedronSubmesh[0].indexBuffer().buffer(),
                                        icosahedronSubmesh[0].indexBuffer().offset(),
                                        NumLights);
    renderEncoder.popDebugGroup();
}

/// Draw the "fairies" at the center of the point lights with a 2D disk using a texture to perform
/// smooth alpha blending on the edges
void LightingSubpass::drawFairies(MTL::RenderCommandEncoder &renderEncoder,
                                  MTL::Buffer& m_lightsData,
                                  MTL::Buffer& m_lightPosition,
                                  MTL::Buffer& m_uniformBuffer,
                                  MTL::Buffer& m_fairy,
                                  MTL::Texture& m_fairyMap) {
    renderEncoder.pushDebugGroup("Draw Fairies");
    renderEncoder.setRenderPipelineState(m_fairyPipelineState);
    renderEncoder.setDepthStencilState(*m_dontWriteDepthStencilState);
    renderEncoder.setCullMode(MTL::CullModeBack);
    renderEncoder.setVertexBuffer(m_uniformBuffer, 0, BufferIndexFrameData);
    renderEncoder.setVertexBuffer(m_fairy, 0, BufferIndexMeshPositions);
    renderEncoder.setVertexBuffer(m_lightsData, 0, BufferIndexLightsData);
    renderEncoder.setVertexBuffer(m_lightPosition, 0, BufferIndexLightsPosition);
    renderEncoder.setFragmentTexture(m_fairyMap, TextureIndexAlpha);
    renderEncoder.drawPrimitives(MTL::PrimitiveTypeTriangleStrip, 0, NumFairyVertices, NumLights);
    renderEncoder.popDebugGroup();
}

/// Draw the sky dome behind all other geometry (testing against depth buffer generated in
///  GBuffer pass)
void LightingSubpass::drawSky(MTL::RenderCommandEncoder &renderEncoder,
                              MTL::Buffer& m_uniformBuffer,
                              Mesh& m_skyMesh,
                              MTL::Texture& m_skyMap) {
    renderEncoder.pushDebugGroup("Draw Sky");
    renderEncoder.setRenderPipelineState(m_skyboxPipelineState);
    renderEncoder.setDepthStencilState(*m_dontWriteDepthStencilState);
    renderEncoder.setCullMode(MTL::CullModeFront);
    
    renderEncoder.setVertexBuffer(m_uniformBuffer, 0, BufferIndexFrameData);
    renderEncoder.setFragmentTexture(m_skyMap, TextureIndexBaseColor);
    
    for (auto &meshBuffer: m_skyMesh.vertexBuffers()) {
        renderEncoder.setVertexBuffer(meshBuffer.buffer(),
                                      meshBuffer.offset(),
                                      meshBuffer.argumentIndex());
    }
    
    
    for (auto &submesh: m_skyMesh.submeshes()) {
        renderEncoder.drawIndexedPrimitives(submesh.primitiveType(),
                                            submesh.indexCount(),
                                            submesh.indexType(),
                                            submesh.indexBuffer().buffer(),
                                            submesh.indexBuffer().offset());
    }
    renderEncoder.popDebugGroup();
}


MTL::Library LightingSubpass::makeShaderLibrary() {
    CFErrorRef error = nullptr;
    CFURLRef libraryURL = nullptr;
    
    libraryURL = CFBundleCopyResourceURL( CFBundleGetMainBundle() , CFSTR("vox.shader"), CFSTR("metallib"), nullptr);
    MTL::Library shaderLibrary = m_device.makeLibrary(libraryURL, &error);
    
    MTLAssert(!error, error, "Could not load Metal shader library");
    
    CFRelease(libraryURL);
    return shaderLibrary;
}

}
