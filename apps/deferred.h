//
//  deferred.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef deferred_hpp
#define deferred_hpp

#include "metal_application.h"

#include "config.h"
#include "mesh.h"
#include "math_utilities.h"

namespace vox {
// The max number of command buffers in flight
static const uint8_t MaxFramesInFlight = 3;

static const float NearPlane = 1;
static const float FarPlane = 150;

// Number of "fairy" lights in scene
static const uint32_t NumLights = 256;

// Number of vertices in our 2D fairy model
static const uint32_t NumFairyVertices = 7;

// 30% of lights are around the tree
// 40% of lights are on the ground inside the columns
// 30% of lights are around the outside of the columns
static const uint32_t TreeLights   = 0            + 0.30 * NumLights;
static const uint32_t GroundLights = TreeLights   + 0.40 * NumLights;
static const uint32_t ColumnLights = GroundLights + 0.30 * NumLights;

class Deferred: public MetalApplication {
public:
    Deferred() = default;
    
    virtual ~Deferred();
    
    /**
     * @brief Additional sample initialization
     */
    virtual bool prepare(Engine &engine) override;
    
    /**
     * @brief Main loop sample events
     */
    virtual void update(float delta_time) override;
    
    virtual void framebufferResize(uint32_t width, uint32_t height) override;
    
private:
    void updateWorldState(uint32_t width, uint32_t height);
    
    void updateLights(const simd::float4x4 &modelViewMatrix);
    
    void loadScene();
    
    void populateLights();
    
    // Vertex descriptor for models loaded with MetalKit
    MTL::VertexDescriptor m_skyVertexDescriptor;
    
    // Array of meshes loaded from the model file
    std::vector<Mesh> *m_meshes;
    // Mesh for an icosahedron used for rendering point lights
    MeshPtr m_icosahedronMesh;
    
    // Projection matrix calculated as a function of view size
    simd::float4x4 m_projection_matrix;
    // Projection matrix used to render the shadow map
    simd::float4x4 m_shadowProjectionMatrix;
    // Current frame number rendering
    uint64_t m_frameNumber;
    
    int8_t m_frameDataBufferIndex;
    
    // Buffers used to store dynamically changing per frame data
    MTL::Buffer m_uniformBuffers[MaxFramesInFlight];
    // Buffers used to story dynamically changing light positions
    MTL::Buffer m_lightPositions[MaxFramesInFlight];
    // Buffer for constant light data
    MTL::Buffer m_lightsData;
    // Light positions before transformation to positions in current frame
    simd::float4 *m_originalLightPositions;
    
    // Mesh buffer for fairies
    MTL::Buffer m_fairy;
    // Texture to create smooth round particles
    MTL::Texture m_fairyMap;
    
private:
    dispatch_semaphore_t m_inFlightSemaphore;
    MTL::CommandBufferHandler *m_completedHandler{nullptr};
    
    MTL::RenderPassDescriptor m_shadowRenderPassDescriptor;
    std::unique_ptr<RenderPass> m_shadowRenderPass{nullptr};
    MTL::Texture m_shadowMap;
    
    MTL::RenderPassDescriptor m_GBufferRenderPassDescriptor;
    std::unique_ptr<RenderPass> m_GBufferRenderPass{nullptr};
    MTL::PixelFormat m_albedo_specular_GBufferFormat;
    MTL::PixelFormat m_normal_shadow_GBufferFormat;
    MTL::PixelFormat m_depth_GBufferFormat;
    MTL::Texture m_albedo_specular_GBuffer;
    MTL::Texture m_normal_shadow_GBuffer;
    MTL::Texture m_depth_GBuffer;
    
    MTL::RenderPassDescriptor m_finalRenderPassDescriptor;
    std::unique_ptr<RenderPass> m_finalRenderPass{nullptr};
};

}

#endif /* deferred_hpp */
