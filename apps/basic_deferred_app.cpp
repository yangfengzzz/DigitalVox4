//
//  basic_deferred_app.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/19.
//

#include "basic_deferred_app.h"
// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"
#include "shader/shader.h"
#include "material/texture_loader.h"
#include "graphics/mesh_loader.h"
#include "graphics/primitive_mesh.h"
#include "sky/skybox_material.h"

namespace vox {
using namespace simd;

bool BasicDeferredApp::prepare(Engine &engine) {
    DeferredApplication::prepare(engine);
    
    // Calculate projection matrix to render shadows
    m_shadowProjectionMatrix = matrix_ortho_left_hand(-53, 53, -33, 53, -53, 53);
    for (uint8_t i = 0; i < MaxFramesInFlight; i++) {
        // Indicate shared storage so that both the CPU can access the buffers
        static const MTL::ResourceOptions storageMode = MTL::ResourceStorageModeShared;
        
        m_uniformBuffers[i] = _device->makeBuffer(sizeof(FrameData), storageMode);
        
        m_uniformBuffers[i].label("UniformBuffer");
        
        m_lightPositions[i] = _device->makeBuffer(sizeof(float4) * NumLights, storageMode);
        
        m_uniformBuffers[i].label("LightPositions");
    }
    return true;
}

void BasicDeferredApp::update(float delta_time) {
    updateWorldState(static_cast<uint32_t>(_renderView->drawableSize().width),
                     static_cast<uint32_t>(_renderView->drawableSize().height));
    _scene->shaderData.setData("frameData", m_uniformBuffers[m_frameDataBufferIndex]);
    _scene->shaderData.setData("lightsData", m_lightsData);
    _scene->shaderData.setData("lightPosition", m_lightPositions[m_frameDataBufferIndex]);
    
    DeferredApplication::update(delta_time);
}

/// Load models/textures, etc.
void BasicDeferredApp::loadScene() {
    Shader::createProperty("frameData", ShaderDataGroup::Scene);
    Shader::createProperty("lightsData", ShaderDataGroup::Scene);
    Shader::createProperty("lightPosition", ShaderDataGroup::Scene);
    
    MeshLoader loader(_device.get());
    loader.loadMesh("../assets/Models", "Temple.obj", _scene.get()->createRootEntity());
    
    // Generate data
    {
        m_lightsData = _device->makeBuffer(sizeof(PointLight) * NumLights);
        
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
        
        m_fairy = _device->makeBuffer(fairyVertices, sizeof(fairyVertices));
        
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
        
        m_icosahedronMesh = PrimitiveMesh::makeIcosahedronMesn(*_device, icosahedronDescriptor, icoshedronRadius);
    }
    
    // Create a sphere for the skybox
    {
        m_skyVertexDescriptor.attributes[VertexAttributePosition].format(MTL::VertexFormatFloat3);
        m_skyVertexDescriptor.attributes[VertexAttributePosition].offset(0);
        m_skyVertexDescriptor.attributes[VertexAttributePosition].bufferIndex(BufferIndexMeshPositions);
        m_skyVertexDescriptor.layouts[BufferIndexMeshPositions].stride(12);
        m_skyVertexDescriptor.attributes[VertexAttributeNormal].format(MTL::VertexFormatFloat3);
        m_skyVertexDescriptor.attributes[VertexAttributeNormal].offset(0);
        m_skyVertexDescriptor.attributes[VertexAttributeNormal].bufferIndex(BufferIndexMeshGenerics);
        m_skyVertexDescriptor.layouts[BufferIndexMeshGenerics].stride(12);
        _scene->background.sky.mesh = PrimitiveMesh::makeSphereMesh(*_device, m_skyVertexDescriptor, 20, 20, 150.0);
    }
    
    // Load textures for non mesh assets
    {
        TextureLoader textureLoader(*_device);
        
        const std::array<std::string, 6> images = {"X+.png", "X-.png", "Y+.png", "Y-.png", "Z+.png", "Z-.png"};
        auto skyMat = std::make_shared<SkyBoxMaterial>();
        skyMat->setTextureCubeMap(textureLoader.loadCubeTexture("../assets/SkyMap", images, true));
        _scene->background.sky.material = skyMat;
        
        m_fairyMap = textureLoader.loadTexture("../assets/Textures/", "fairy.png", true);
        m_fairyMap.label("Fairy Map");
    }
}

void BasicDeferredApp::framebufferResize(uint32_t width, uint32_t height) {
    // When reshape is called, update the aspect ratio and projection matrix since the view
    //   orientation or size has changed
    float aspect = (float) width / (float) height;
    m_projection_matrix = matrix_perspective_left_hand(65.0f * (M_PI / 180.0f), aspect, NearPlane, FarPlane);
}

//MARK: - TODO Warpper into Scene and Script
/// Update application state for the current frame
void BasicDeferredApp::updateWorldState(uint32_t width, uint32_t height) {
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
void BasicDeferredApp::updateLights(const float4x4 &modelViewMatrix) {
    PointLight *lightData = (PointLight *) m_lightsData.contents();
    
    float4 *currentBuffer =
    (float4 *) m_lightPositions[m_frameDataBufferIndex].contents();
    
    float4 *originalLightPositions = (float4 *) m_originalLightPositions;
    
    for (int i = 0; i < NumLights; i++) {
        float4 currentPosition;
        
        if (i < TreeLights) {
            double lightPeriod = lightData[i].light_speed * m_frameNumber;
            lightPeriod += originalLightPositions[i].y;
            lightPeriod -= std::floor(lightPeriod);  // Get fractional part
            
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

/// Initialize light positions and colors
void BasicDeferredApp::populateLights() {
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
