//
//  deferred.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/14.
//

#include "deferred.h"
// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"
#include "rendering/render_pass.h"
#include "rendering/subpasses/shadow_subpass.h"
#include "rendering/subpasses/deferred_subpass.h"
#include "rendering/subpasses/compose_subpass.h"
#include "rendering/subpasses/point_light_subpass.h"
#include "rendering/subpasses/skybox_subpass.h"
#include "rendering/subpasses/particle_subpass.h"
#include "sky/skybox_material.h"

#include "engine.h"
#include "core/CPPMetalAssert.hpp"
#include "material/texture_loader.h"
#include "graphics/mesh_loader.h"
#include "graphics/primitive_mesh.h"
#include "shader/shader.h"

namespace vox {
using namespace simd;

Deferred::~Deferred() {
    delete m_completedHandler;
    render_pipeline.reset();
}

bool Deferred::prepare(Engine &engine) {
    MetalApplication::prepare(engine);
    MTL::Library shaderLibrary = makeShaderLibrary();
    auto extent = engine.get_window().get_extent();
    render_context->resize(MTL::SizeMake(extent.width * 2, extent.height * 2, 0));
    render_context->depthStencilPixelFormat(MTL::PixelFormatDepth32Float_Stencil8);
    render_context->colorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
    
    m_inFlightSemaphore = dispatch_semaphore_create(MaxFramesInFlight);
    
#pragma mark VertexDescriptor
    for (uint8_t i = 0; i < MaxFramesInFlight; i++) {
        // Indicate shared storage so that both the CPU can access the buffers
        static const MTL::ResourceOptions storageMode = MTL::ResourceStorageModeShared;
        
        m_uniformBuffers[i] = device->makeBuffer(sizeof(FrameData), storageMode);
        
        m_uniformBuffers[i].label("UniformBuffer");
        
        m_lightPositions[i] = device->makeBuffer(sizeof(float4) * NumLights, storageMode);
        
        m_uniformBuffers[i].label("LightPositions");
    }
    
    // Calculate projection matrix to render shadows
    {
        m_shadowProjectionMatrix = matrix_ortho_left_hand(-53, 53, -33, 53, -53, 53);
    }
    
    {
        // Positions.
        m_defaultVertexDescriptor.attributes[VertexAttributePosition].format(MTL::VertexFormatFloat3);
        m_defaultVertexDescriptor.attributes[VertexAttributePosition].offset(0);
        m_defaultVertexDescriptor.attributes[VertexAttributePosition].bufferIndex(BufferIndexMeshPositions);
        
        // Texture coordinates.
        m_defaultVertexDescriptor.attributes[VertexAttributeTexcoord].format(MTL::VertexFormatFloat2);
        m_defaultVertexDescriptor.attributes[VertexAttributeTexcoord].offset(0);
        m_defaultVertexDescriptor.attributes[VertexAttributeTexcoord].bufferIndex(BufferIndexMeshGenerics);
        
        // Normals.
        m_defaultVertexDescriptor.attributes[VertexAttributeNormal].format(MTL::VertexFormatHalf4);
        m_defaultVertexDescriptor.attributes[VertexAttributeNormal].offset(8);
        m_defaultVertexDescriptor.attributes[VertexAttributeNormal].bufferIndex(BufferIndexMeshGenerics);
        
        // Tangents
        m_defaultVertexDescriptor.attributes[VertexAttributeTangent].format(MTL::VertexFormatHalf4);
        m_defaultVertexDescriptor.attributes[VertexAttributeTangent].offset(16);
        m_defaultVertexDescriptor.attributes[VertexAttributeTangent].bufferIndex(BufferIndexMeshGenerics);
        
        // Bitangents
        m_defaultVertexDescriptor.attributes[VertexAttributeBitangent].format(MTL::VertexFormatHalf4);
        m_defaultVertexDescriptor.attributes[VertexAttributeBitangent].offset(24);
        m_defaultVertexDescriptor.attributes[VertexAttributeBitangent].bufferIndex(BufferIndexMeshGenerics);
        
        // Position Buffer Layout
        m_defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stride(12);
        m_defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stepRate(1);
        m_defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stepFunction(MTL::VertexStepFunctionPerVertex);
        
        // Generic Attribute Buffer Layout
        m_defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stride(32);
        m_defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stepRate(1);
        m_defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stepFunction(MTL::VertexStepFunctionPerVertex);
        
        m_skyVertexDescriptor.attributes[VertexAttributePosition].format(MTL::VertexFormatFloat3);
        m_skyVertexDescriptor.attributes[VertexAttributePosition].offset(0);
        m_skyVertexDescriptor.attributes[VertexAttributePosition].bufferIndex(BufferIndexMeshPositions);
        m_skyVertexDescriptor.layouts[BufferIndexMeshPositions].stride(12);
        m_skyVertexDescriptor.attributes[VertexAttributeNormal].format(MTL::VertexFormatFloat3);
        m_skyVertexDescriptor.attributes[VertexAttributeNormal].offset(0);
        m_skyVertexDescriptor.attributes[VertexAttributeNormal].bufferIndex(BufferIndexMeshGenerics);
        m_skyVertexDescriptor.layouts[BufferIndexMeshGenerics].stride(12);
    }
    
    loadScene();
    Shader::createProperty("frameData", ShaderDataGroup::Scene);
    Shader::createProperty("lightsData", ShaderDataGroup::Scene);
    Shader::createProperty("lightPosition", ShaderDataGroup::Scene);
    
#pragma mark Shadow render pass descriptor setup
    {
        MTL::TextureDescriptor shadowTextureDesc;
        shadowTextureDesc.pixelFormat(MTL::PixelFormatDepth16Unorm);
        shadowTextureDesc.width(2048);
        shadowTextureDesc.height(2048);
        shadowTextureDesc.mipmapLevelCount(1);
        shadowTextureDesc.resourceOptions(MTL::ResourceStorageModePrivate);
        shadowTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        
        m_shadowMap = device->makeTexture(shadowTextureDesc);
        m_shadowMap.label("Shadow Map");
        m_shadowRenderPassDescriptor.depthAttachment.texture( m_shadowMap );
        m_shadowRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
        m_shadowRenderPassDescriptor.depthAttachment.storeAction(MTL::StoreActionStore);
        m_shadowRenderPassDescriptor.depthAttachment.clearDepth(1.0);
        m_shadowRenderPass = std::make_unique<RenderPass>(&m_shadowRenderPassDescriptor);
        m_shadowRenderPass->addSubpass(std::make_unique<ShadowSubpass>(&m_shadowRenderPassDescriptor, scene.get(), nullptr,
                                                                       shaderLibrary, *device));
    }
    
#pragma mark GBuffer render pass descriptor setup
    {
        m_albedo_specular_GBufferFormat = MTL::PixelFormatRGBA8Unorm_sRGB;
        m_normal_shadow_GBufferFormat = MTL::PixelFormatRGBA8Snorm;
        m_depth_GBufferFormat = MTL::PixelFormatR32Float;
        MTL::TextureDescriptor GBufferTextureDesc;
        
        GBufferTextureDesc.pixelFormat(MTL::PixelFormatRGBA8Unorm_sRGB);
        GBufferTextureDesc.width(extent.width * 2);
        GBufferTextureDesc.height(extent.height * 2);
        GBufferTextureDesc.mipmapLevelCount(1);
        GBufferTextureDesc.textureType(MTL::TextureType2D);
        GBufferTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        GBufferTextureDesc.storageMode(MTL::StorageModePrivate);
        GBufferTextureDesc.pixelFormat(m_albedo_specular_GBufferFormat);
        m_albedo_specular_GBuffer = device->makeTexture(GBufferTextureDesc);
        m_albedo_specular_GBuffer.label("Albedo + Shadow GBuffer");
        GBufferTextureDesc.pixelFormat(m_normal_shadow_GBufferFormat);
        m_normal_shadow_GBuffer = device->makeTexture(GBufferTextureDesc);
        m_normal_shadow_GBuffer.label("Normal + Specular GBuffer");
        GBufferTextureDesc.pixelFormat(m_depth_GBufferFormat);
        m_depth_GBuffer = device->makeTexture(GBufferTextureDesc);
        m_depth_GBuffer.label("Depth GBuffer");
        
        // Create a render pass descriptor to create an encoder for rendering to the GBuffers.
        // The encoder stores rendered data of each attachment when encoding ends.
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetLighting].loadAction(MTL::LoadActionDontCare);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetLighting].storeAction(MTL::StoreActionDontCare);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].loadAction(MTL::LoadActionDontCare);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].storeAction(MTL::StoreActionStore);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].texture(m_albedo_specular_GBuffer);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].loadAction(MTL::LoadActionDontCare);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].storeAction(MTL::StoreActionStore);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].texture(m_normal_shadow_GBuffer);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].loadAction(MTL::LoadActionDontCare);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].storeAction(MTL::StoreActionStore);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].texture(m_depth_GBuffer);
        m_GBufferRenderPassDescriptor.depthAttachment.clearDepth(1.0);
        m_GBufferRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
        m_GBufferRenderPassDescriptor.depthAttachment.storeAction(MTL::StoreActionStore);
        m_GBufferRenderPassDescriptor.depthAttachment.texture(*render_context->depthStencilTexture());
        m_GBufferRenderPassDescriptor.stencilAttachment.clearStencil(0);
        m_GBufferRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionClear);
        m_GBufferRenderPassDescriptor.stencilAttachment.storeAction(MTL::StoreActionStore);
        m_GBufferRenderPassDescriptor.stencilAttachment.texture(*render_context->depthStencilTexture());
        m_GBufferRenderPass = std::make_unique<RenderPass>(&m_GBufferRenderPassDescriptor);
        m_GBufferRenderPass->addSubpass(std::make_unique<DeferredSubpass>(&m_GBufferRenderPassDescriptor, scene.get(), nullptr,
                                                                          shaderLibrary, *device, &m_shadowRenderPassDescriptor));
    }
    
#pragma mark Compositor render pass descriptor setup
    {
        // Create a render pass descriptor for thelighting and composition pass
        // Whatever rendered in the final pass needs to be stored so it can be displayed
        m_finalRenderPassDescriptor.colorAttachments[0].storeAction(MTL::StoreActionStore);
        m_finalRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionLoad);
        m_finalRenderPassDescriptor.depthAttachment.texture(*render_context->depthStencilTexture());
        m_finalRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionLoad);
        m_finalRenderPassDescriptor.stencilAttachment.texture(*render_context->depthStencilTexture());
        m_finalRenderPass = std::make_unique<RenderPass>(&m_finalRenderPassDescriptor);
        m_finalRenderPass->addSubpass(std::make_unique<ComposeSubpass>(&m_finalRenderPassDescriptor, scene.get(), nullptr,
                                                                       shaderLibrary, *device, MTL::PixelFormatBGRA8Unorm_sRGB,
                                                                       &m_GBufferRenderPassDescriptor));
        m_finalRenderPass->addSubpass(std::make_unique<PointLightSubpass>(&m_finalRenderPassDescriptor, scene.get(), nullptr,
                                                                          shaderLibrary, *device, MTL::PixelFormatBGRA8Unorm_sRGB,
                                                                          m_icosahedronMesh, &m_GBufferRenderPassDescriptor, NumLights));
        m_finalRenderPass->addSubpass(std::make_unique<SkyboxSubpass>(&m_finalRenderPassDescriptor, scene.get(), nullptr,
                                                                      shaderLibrary, *device, MTL::PixelFormatBGRA8Unorm_sRGB));
        m_finalRenderPass->addSubpass(std::make_unique<ParticleSubpass>(&m_finalRenderPassDescriptor, scene.get(), nullptr,
                                                                        shaderLibrary, *device, MTL::PixelFormatBGRA8Unorm_sRGB,
                                                                        m_fairy, m_fairyMap, NumLights, NumFairyVertices));
    }
    framebuffer_resize(extent.width*2, extent.height*2);
    
    return true;
}

void Deferred::update(float delta_time) {
    MetalApplication::update(delta_time);
    
    // Wait to ensure only MaxFramesInFlight are getting processed by any stage in the Metal
    // pipeline (App, Metal, Drivers, GPU, etc)
    dispatch_semaphore_wait(m_inFlightSemaphore, DISPATCH_TIME_FOREVER);
    updateWorldState(static_cast<uint32_t>(render_context->drawableSize().width),
                     static_cast<uint32_t>(render_context->drawableSize().height));
    scene->shaderData.setData("frameData", m_uniformBuffers[m_frameDataBufferIndex]);
    scene->shaderData.setData("lightsData", m_lightsData);
    scene->shaderData.setData("lightPosition", m_lightPositions[m_frameDataBufferIndex]);
    
    {
        // Create a new command buffer for each render pass to the current drawable
        MTL::CommandBuffer commandBuffer = m_commandQueue.commandBuffer();
        commandBuffer.label("Shadow & GBuffer Commands");
        
        m_shadowRenderPass->draw(commandBuffer);
        m_GBufferRenderPass->draw(commandBuffer, "GBuffer Generation");
        
        // Commit commands so that Metal can begin working on non-drawable dependant work without
        // waiting for a drawable to become avaliable
        commandBuffer.commit();
    }
    
    {
        MTL::CommandBuffer commandBuffer = m_commandQueue.commandBuffer();
        if (!m_completedHandler) {
            // Create a completed handler functor for Metal to execute when the GPU has fully finished
            // processing the commands encoded for this frame.  This implenentation of the completed
            // hander signals the `m_inFlightSemaphore`, which indicates that the GPU is no longer
            // accesing the the dynamic buffer written this frame.  When the GPU no longer accesses the
            // buffer, the Subpass can safely overwrite the buffer's data to update data for a future
            // frame.
            struct CommandBufferCompletedHandler : public MTL::CommandBufferHandler {
                dispatch_semaphore_t semaphore;
                
                void operator()(const MTL::CommandBuffer &) {
                    dispatch_semaphore_signal(semaphore);
                }
            };
            
            CommandBufferCompletedHandler *completedHandler = new CommandBufferCompletedHandler();
            completedHandler->semaphore = m_inFlightSemaphore;
            
            m_completedHandler = completedHandler;
        }
        commandBuffer.addCompletedHandler(*m_completedHandler);
        commandBuffer.label("Lighting Commands");
        
        MTL::Drawable *drawable = render_context->currentDrawable();
        // The final pass can only render if a drawable is available, otherwise it needs to skip
        // rendering this frame.
        if (drawable) {
            // Render the lighting and composition pass
            m_finalRenderPassDescriptor.colorAttachments[0].texture(*drawable->texture());
            m_finalRenderPassDescriptor.depthAttachment.texture(*render_context->depthStencilTexture());
            m_finalRenderPassDescriptor.stencilAttachment.texture(*render_context->depthStencilTexture());
            m_finalRenderPass->draw(commandBuffer, "Lighting & Composition Pass");
        }
        
        // Schedule a present once the framebuffer is complete using the current drawable
        if (render_context->currentDrawable()) {
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
            
            PresentationScheduledHandler *scheduledHandler = new PresentationScheduledHandler(*render_context->currentDrawable());
            commandBuffer.addScheduledHandler(*scheduledHandler);
        }
        
        // Finalize rendering here & push the command buffer to the GPU
        commandBuffer.commit();
    }
}

void Deferred::framebuffer_resize(uint32_t width, uint32_t height) {
    // When reshape is called, update the aspect ratio and projection matrix since the view
    //   orientation or size has changed
    float aspect = (float) width / (float) height;
    m_projection_matrix = matrix_perspective_left_hand(65.0f * (M_PI / 180.0f), aspect, NearPlane, FarPlane);
    
    MetalApplication::framebuffer_resize(width, height);
    m_GBufferRenderPassDescriptor.depthAttachment.texture(*render_context->depthStencilTexture());
    m_GBufferRenderPassDescriptor.stencilAttachment.texture(*render_context->depthStencilTexture());
    
    MTL::TextureDescriptor GBufferTextureDesc;
    GBufferTextureDesc.pixelFormat(MTL::PixelFormatRGBA8Unorm_sRGB);
    GBufferTextureDesc.width(width);
    GBufferTextureDesc.height(height);
    GBufferTextureDesc.mipmapLevelCount(1);
    GBufferTextureDesc.textureType(MTL::TextureType2D);
    GBufferTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    GBufferTextureDesc.storageMode(MTL::StorageModePrivate);
    GBufferTextureDesc.pixelFormat(m_albedo_specular_GBufferFormat);
    m_albedo_specular_GBuffer = device->makeTexture(GBufferTextureDesc);
    m_albedo_specular_GBuffer.label("Albedo + Shadow GBuffer");
    GBufferTextureDesc.pixelFormat(m_normal_shadow_GBufferFormat);
    m_normal_shadow_GBuffer = device->makeTexture(GBufferTextureDesc);
    m_normal_shadow_GBuffer.label("Normal + Specular GBuffer");
    GBufferTextureDesc.pixelFormat(m_depth_GBufferFormat);
    m_depth_GBuffer = device->makeTexture(GBufferTextureDesc);
    m_depth_GBuffer.label("Depth GBuffer");
    // Re-set GBuffer textures in the GBuffer render pass descriptor after they have been
    // reallocated by a resize
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].texture(m_albedo_specular_GBuffer);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].texture(m_normal_shadow_GBuffer);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].texture(m_depth_GBuffer);
}

//MARK: - TODO Warpper into Scene and Script
/// Update application state for the current frame
void Deferred::updateWorldState(uint32_t width, uint32_t height) {
    m_frameNumber++;
    m_frameDataBufferIndex = (m_frameDataBufferIndex + 1) % MaxFramesInFlight;
    
    FrameData *frameData = (FrameData *) (m_uniformBuffers[m_frameDataBufferIndex].contents());
    
    // Set projection matrix and calculate inverted projection matrix
    frameData->projection_matrix = m_projection_matrix;
    frameData->projection_matrix_inverse = matrix_invert(m_projection_matrix);
    
    // Set screen dimensions
    frameData->framebuffer_width = width;
    frameData->framebuffer_height = height;
    
    frameData->shininess_factor = 1;
    frameData->fairy_specular_intensity = 32;
    
    float cameraRotationRadians = m_frameNumber * 0.0025f + M_PI;
    
    float3 cameraRotationAxis = {0, 1, 0};
    float4x4 cameraRotationMatrix = matrix4x4_rotation(cameraRotationRadians, cameraRotationAxis);
    
    float4x4 view_matrix = matrix_look_at_left_hand(0, 18, -50,
                                                    0, 5, 0,
                                                    0, 1, 0);
    
    view_matrix = view_matrix * cameraRotationMatrix;
    
    frameData->view_matrix = view_matrix;
    
    float4x4 templeScaleMatrix = matrix4x4_scale(0.1, 0.1, 0.1);
    float4x4 templeTranslateMatrix = matrix4x4_translation(0, -10, 0);
    float4x4 templeModelMatrix = templeTranslateMatrix * templeScaleMatrix;
    frameData->temple_model_matrix = templeModelMatrix;
    frameData->temple_modelview_matrix = frameData->view_matrix * templeModelMatrix;
    frameData->temple_normal_matrix = matrix3x3_upper_left(frameData->temple_model_matrix);
    
    float skyRotation = m_frameNumber * 0.005f - (M_PI_4 * 3);
    
    float3 skyRotationAxis = {0, 1, 0};
    float4x4 skyModelMatrix = matrix4x4_rotation(skyRotation, skyRotationAxis);
    frameData->sky_modelview_matrix = cameraRotationMatrix * skyModelMatrix;
    
    // Update directional light color
    float4 sun_color = {0.5, 0.5, 0.5, 1.0};
    frameData->sun_color = sun_color;
    frameData->sun_specular_intensity = 1;
    
    // Update sun direction in view space
    float4 sunModelPosition = {-0.25, -0.5, 1.0, 0.0};
    
    float4 sunWorldPosition = skyModelMatrix * sunModelPosition;
    
    float4 sunWorldDirection = -sunWorldPosition;
    
    frameData->sun_eye_direction = view_matrix * sunWorldDirection;
    
    {
        float4 directionalLightUpVector = {0.0, 1.0, 1.0, 1.0};
        
        directionalLightUpVector = skyModelMatrix * directionalLightUpVector;
        directionalLightUpVector.xyz = normalize(directionalLightUpVector.xyz);
        
        float4x4 shadowViewMatrix = matrix_look_at_left_hand(sunWorldDirection.xyz / 10,
                                                             (float3) {0, 0, 0},
                                                             directionalLightUpVector.xyz);
        
        float4x4 shadowModelViewMatrix = shadowViewMatrix * templeModelMatrix;
        
        frameData->shadow_mvp_matrix = m_shadowProjectionMatrix * shadowModelViewMatrix;
    }
    
    {
        // When calculating texture coordinates to sample from shadow map, flip the y/t coordinate and
        // convert from the [-1, 1] range of clip coordinates to [0, 1] range of
        // used for texture sampling
        float4x4 shadowScale = matrix4x4_scale(0.5f, -0.5f, 1.0);
        float4x4 shadowTranslate = matrix4x4_translation(0.5, 0.5, 0);
        float4x4 shadowTransform = shadowTranslate * shadowScale;
        
        frameData->shadow_mvp_xform_matrix = shadowTransform * frameData->shadow_mvp_matrix;
    }
    
    frameData->fairy_size = .4;
    
    updateLights(frameData->temple_modelview_matrix);
}

/// Update light positions
void Deferred::updateLights(const float4x4 &modelViewMatrix) {
    PointLight *lightData = (PointLight *) m_lightsData.contents();
    
    float4 *currentBuffer =
    (float4 *) m_lightPositions[m_frameDataBufferIndex].contents();
    
    float4 *originalLightPositions = (float4 *) m_originalLightPositions;
    
    for (int i = 0; i < NumLights; i++) {
        float4 currentPosition;
        
        if (i < TreeLights) {
            double lightPeriod = lightData[i].light_speed * m_frameNumber;
            lightPeriod += originalLightPositions[i].y;
            lightPeriod -= floor(lightPeriod);  // Get fractional part
            
            // Use pow to slowly move the light outward as it reaches the branches of the tree
            float r = 1.2 + 10.0 * powf(lightPeriod, 5.0);
            
            currentPosition.x = originalLightPositions[i].x * r;
            currentPosition.y = 200.0f + lightPeriod * 400.0f;
            currentPosition.z = originalLightPositions[i].z * r;
            currentPosition.w = 1;
        } else {
            float rotationRadians = lightData[i].light_speed * m_frameNumber;
            float4x4 rotation = matrix4x4_rotation(rotationRadians, 0, 1, 0);
            currentPosition = rotation * originalLightPositions[i];
        }
        
        currentPosition = modelViewMatrix * currentPosition;
        currentBuffer[i] = currentPosition;
    }
}

/// Load models/textures, etc.
void Deferred::loadScene() {
    newMeshesFromBundlePath("../assets/Models", "Temple.obj",
                            *device, scene.get(), m_defaultVertexDescriptor);
        
    // Generate data
    {
        m_lightsData = device->makeBuffer(sizeof(PointLight) * NumLights);
        
        m_lightsData.label("LightData");
        
        populateLights();
    }
    
    // Create a simple 2D triangle strip circle mesh for fairies
    {
        SimpleVertex fairyVertices[NumFairyVertices];
        const float angle = 2 * M_PI / (float) NumFairyVertices;
        for (int vtx = 0; vtx < NumFairyVertices; vtx++) {
            int point = (vtx % 2) ? (vtx + 1) / 2 : -vtx / 2;
            float2 position = {sin(point * angle), cos(point * angle)};
            fairyVertices[vtx].position = position;
        }
        
        m_fairy = device->makeBuffer(fairyVertices, sizeof(fairyVertices));
        
        m_fairy.label("Fairy Vertices");
    }
    
    // Create an icosahedron mesh for fairy light volumes
    {
        // Create vertex descriptor with layout for icoshedron
        MTL::VertexDescriptor icosahedronDescriptor;
        icosahedronDescriptor.attributes[VertexAttributePosition].format(MTL::VertexFormatFloat4);
        icosahedronDescriptor.attributes[VertexAttributePosition].offset(0);
        icosahedronDescriptor.attributes[VertexAttributePosition].bufferIndex(BufferIndexMeshPositions);
        
        icosahedronDescriptor.layouts[BufferIndexMeshPositions].stride(sizeof(float4));
        
        // Calculate radius such that minium radius of icosahedronDescriptor is 1
        const float icoshedronRadius = 1.0 / (sqrtf(3.0) / 12.0 * (3.0 + sqrtf(5.0)));
        
        m_icosahedronMesh = PrimitiveMesh::makeIcosahedronMesn(*device, icosahedronDescriptor, icoshedronRadius);
    }
    
    // Create a sphere for the skybox
    {
        scene->background.sky.mesh = PrimitiveMesh::makeSphereMesh(*device, m_skyVertexDescriptor, 20, 20, 150.0);
    }
    
    // Load textures for non mesh assets
    {
        TextureLoader textureLoader(*device);
        
        const std::array<std::string, 6> images = {"X+.png", "X-.png", "Y+.png", "Y-.png", "Z+.png", "Z-.png"};
        auto skyMat = std::make_shared<SkyBoxMaterial>();
        skyMat->setTextureCubeMap(textureLoader.loadCubeTexture("../assets/SkyMap", images, true));
        scene->background.sky.material = skyMat;
        
        m_fairyMap = textureLoader.loadTexture("../assets/Textures/", "fairy.png", true);
        m_fairyMap.label("Fairy Map");
    }
}

/// Initialize light positions and colors
void Deferred::populateLights() {
    PointLight *light_data = (PointLight *) m_lightsData.contents();
    
    m_originalLightPositions = new float4[NumLights];
    
    float4 *light_position = m_originalLightPositions;
    
    srandom(0x134e5348);
    
    for (uint32_t lightId = 0; lightId < NumLights; lightId++) {
        float distance = 0;
        float height = 0;
        float angle = 0;
        float speed = 0;
        
        if (lightId < TreeLights) {
            distance = random_float(38, 42);
            height = random_float(0, 1);
            angle = random_float(0, M_PI * 2);
            speed = random_float(0.003, 0.014);
        } else if (lightId < GroundLights) {
            distance = random_float(140, 260);
            height = random_float(140, 150);
            angle = random_float(0, M_PI * 2);
            speed = random_float(0.006, 0.027);
            speed *= (random() % 2) * 2 - 1;
        } else if (lightId < ColumnLights) {
            distance = random_float(365, 380);
            height = random_float(150, 190);
            angle = random_float(0, M_PI * 2);
            speed = random_float(0.004, 0.014);
            speed *= (random() % 2) * 2 - 1;
        }
        
        speed *= .5;
        *light_position = (float4) {distance * sinf(angle), height, distance * cosf(angle), 1};
        light_data->light_radius = random_float(25, 35) / 10.0;
        light_data->light_speed = speed;
        
        int colorId = random() % 3;
        if (colorId == 0) {
            light_data->light_color = (float3) {random_float(4, 6), random_float(0, 4), random_float(0, 4)};
        } else if (colorId == 1) {
            light_data->light_color = (float3) {random_float(0, 4), random_float(4, 6), random_float(0, 4)};
        } else {
            light_data->light_color = (float3) {random_float(0, 4), random_float(0, 4), random_float(4, 6)};
        }
        
        light_data++;
        light_position++;
    }
}



}
