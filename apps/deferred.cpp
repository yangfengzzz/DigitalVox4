//
//  deferred.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/14.
//

#include "deferred.h"
// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"
#include "rendering/lighting_subpass.h"
#include "engine.h"
#include "core/CPPMetalAssert.hpp"
#include "material/texture_loader.h"
#include "graphics/mesh_loader.h"
#include "graphics/primitive_mesh.h"

namespace vox {
using namespace simd;

Deferred::~Deferred() {
    delete m_completedHandler;
    render_pipeline.reset();
}

bool Deferred::prepare(Engine &engine) {
    MetalApplication::prepare(engine);
    
    m_inFlightSemaphore = dispatch_semaphore_create(MaxFramesInFlight);
    
#pragma mark Shadow render pass descriptor setup
    {
        // m_shadowRenderPassDescriptor.depthAttachment.texture(m_shadowMap);
        m_shadowRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
        m_shadowRenderPassDescriptor.depthAttachment.storeAction(MTL::StoreActionStore);
        m_shadowRenderPassDescriptor.depthAttachment.clearDepth(1.0);
    }
    
    {
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
    }
    
    {
        // Create a render pass descriptor for thelighting and composition pass
        // Whatever rendered in the final pass needs to be stored so it can be displayed
        m_finalRenderPassDescriptor.colorAttachments[0].storeAction(MTL::StoreActionStore);
        m_finalRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionLoad);
        m_finalRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionLoad);
    }
    
    render_pipeline = std::make_unique<LightingSubpass>(render_context.get());
    
    auto extent = engine.get_window().get_extent();
    framebuffer_resize(extent.width*2, extent.height*2);
    
#pragma mark VertexDescriptor
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
    
    return true;
}

void Deferred::update(float delta_time) {
    MetalApplication::update(delta_time);
    
    auto subpass = static_cast<LightingSubpass*>(render_pipeline.get());
    {
        // Wait to ensure only MaxFramesInFlight are getting processed by any stage in the Metal
        // pipeline (App, Metal, Drivers, GPU, etc)
        dispatch_semaphore_wait(m_inFlightSemaphore, DISPATCH_TIME_FOREVER);
        // Create a new command buffer for each render pass to the current drawable
        MTL::CommandBuffer commandBuffer = m_commandQueue.commandBuffer();
        commandBuffer.label("Shadow & GBuffer Commands");
        
        subpass->updateWorldState();
        subpass->drawShadow(commandBuffer);
        
        m_GBufferRenderPassDescriptor.depthAttachment.texture(*render_context->depthStencilTexture());
        m_GBufferRenderPassDescriptor.stencilAttachment.texture(*render_context->depthStencilTexture());
        
        MTL::RenderCommandEncoder renderEncoder =
        commandBuffer.renderCommandEncoderWithDescriptor(m_GBufferRenderPassDescriptor);
        renderEncoder.label("GBuffer Generation");
        
        subpass->drawGBuffer(renderEncoder);
        
        renderEncoder.endEncoding();
        
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
            
            MTL::RenderCommandEncoder renderEncoder =
            commandBuffer.renderCommandEncoderWithDescriptor(m_finalRenderPassDescriptor);
            renderEncoder.label("Lighting & Composition Pass");
            
            subpass->drawDirectionalLight(renderEncoder);
            
            subpass->drawPointLightMask(renderEncoder);
            
            subpass->drawPointLights(renderEncoder);
            
            subpass->drawSky(renderEncoder);
            
            subpass->drawFairies(renderEncoder);
            
            renderEncoder.endEncoding();
        }
        
        subpass->endFrame(commandBuffer);
    }
}

void Deferred::framebuffer_resize(uint32_t width, uint32_t height) {
    MetalApplication::framebuffer_resize(width, height);
    
    auto subpass = static_cast<LightingSubpass*>(render_pipeline.get());
    // The subpass base class allocates all GBuffers >except< lighting GBuffer (since with the
    // single-pass deferred subpass the lighting buffer is the same as the drawable)
    subpass->drawableSizeWillChange(MTL::SizeMake(width, height, 0), MTL::StorageModePrivate);
    
    // Re-set GBuffer textures in the GBuffer render pass descriptor after they have been
    // reallocated by a resize
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].texture(subpass->m_albedo_specular_GBuffer);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].texture(subpass->m_normal_shadow_GBuffer);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].texture(subpass->m_depth_GBuffer);
}

//MARK: - TODO Warpper into Scene and Script
// Number of vertices in our 2D fairy model
static const uint32_t NumFairyVertices = 7;
// 30% of lights are around the tree
// 40% of lights are on the ground inside the columns
// 30% of lights are around the outside of the columns
static const uint32_t TreeLights = 0 + 0.30 * NumLights;
static const uint32_t GroundLights = TreeLights + 0.40 * NumLights;
static const uint32_t ColumnLights = GroundLights + 0.30 * NumLights;

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
    // Create and load assets into Metal objects including meshes and textures
    CFErrorRef error = nullptr;
    
    m_meshes = newMeshesFromBundlePath("../assets/Models", "Temple.obj",
                                       *device, m_defaultVertexDescriptor, &error);
    
    MTLAssert(m_meshes, error, "Could not create meshes from model file");
    
    // Generate data
    {
        m_lightsData = device->makeBuffer(sizeof(PointLight) * NumLights);
        
        m_lightsData.label("LightData");
        
        populateLights();
    }
    
    // Create quad for fullscreen composition drawing
    {
        static const SimpleVertex QuadVertices[] =
        {
            {{-1.0f, -1.0f,}},
            {{-1.0f, 1.0f,}},
            {{1.0f, -1.0f,}},
            
            {{1.0f, -1.0f,}},
            {{-1.0f, 1.0f,}},
            {{1.0f, 1.0f,}},
        };
        
        m_quadVertexBuffer = device->makeBuffer(QuadVertices, sizeof(QuadVertices));
        
        m_quadVertexBuffer.label("Quad Vertices");
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
        m_skyMesh = PrimitiveMesh::makeSphereMesh(*device, m_skyVertexDescriptor, 20, 20, 150.0);
    }
    
    // Load textures for non mesh assets
    {
        TextureLoader textureLoader(*device);
        
        TextureLoaderOptions textureLoaderOptions;
        
        textureLoaderOptions.usage = MTL::TextureUsageShaderRead;
        textureLoaderOptions.storageMode = MTL::StorageModePrivate;
        
        const std::array<std::string, 6> images = {"X+.png", "X-.png", "Y+.png", "Y-.png", "Z+.png", "Z-.png"};
        m_skyMap = textureLoader.loadCubeTexture("../assets/SkyMap", images, true);
        
//        m_skyMap = textureLoader.makeTexture("SkyMap",
//                                             1.0,
//                                             textureLoaderOptions,
//                                             &error);
        
        MTLAssert(error == nullptr, error, "Could not load sky texture");
        
        m_skyMap.label("Sky Map");
        
        m_fairyMap = textureLoader.loadTexture("../assets/Textures/", "fairy.png", true);
        
//        m_fairyMap = textureLoader.makeTexture("file://../assets/Textures/fairy.png",
//                                               textureLoaderOptions,
//                                               &error);
        
        MTLAssert(error == nullptr, error, "Could not load fairy texture");
        
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
