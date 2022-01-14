//
//  deferred.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef deferred_hpp
#define deferred_hpp

#include "metal_application.h"

namespace vox {
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
    
    virtual void framebuffer_resize(uint32_t width, uint32_t height) override;
    
private:
    void updateWorldState(uint32_t width, uint32_t height);
    
    void updateLights(const simd::float4x4 &modelViewMatrix);
    
    void loadScene();
    
    void populateLights();

    // Vertex descriptor for models loaded with MetalKit
    MTL::VertexDescriptor m_defaultVertexDescriptor;
    // Vertex descriptor for models loaded with MetalKit
    MTL::VertexDescriptor m_skyVertexDescriptor;
    
    // Array of meshes loaded from the model file
    std::vector<Mesh> *m_meshes;
    // Mesh for an icosahedron used for rendering point lights
    Mesh m_icosahedronMesh;
    
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
    // Mesh buffer for simple Quad
    MTL::Buffer m_quadVertexBuffer;
    // Light positions before transformation to positions in current frame
    simd::float4 *m_originalLightPositions;
    
    // Mesh buffer for fairies
    MTL::Buffer m_fairy;
    // Texture to create smooth round particles
    MTL::Texture m_fairyMap;
    
    // Mesh for sphere use to render the skybox
    Mesh m_skyMesh;
    // Texture for skybox
    MTL::Texture m_skyMap;
    
private:
    dispatch_semaphore_t m_inFlightSemaphore;
    MTL::CommandBufferHandler *m_completedHandler{nullptr};

    MTL::RenderPassDescriptor m_shadowRenderPassDescriptor;
    MTL::RenderPassDescriptor m_GBufferRenderPassDescriptor;
    MTL::RenderPassDescriptor m_finalRenderPassDescriptor;
};

}

#endif /* deferred_hpp */
