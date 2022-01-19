//
//  basic_deferred_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/19.
//

#ifndef basic_deferred_app_hpp
#define basic_deferred_app_hpp

#include "deferred_application.h"
#include "math_utilities.h"

namespace vox {
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

class BasicDeferredApp : public DeferredApplication {
public:
    bool prepare(Engine &engine) override;
    
    void update(float delta_time) override;

    void loadScene() override;
    
    void framebufferResize(uint32_t width, uint32_t height) override;

private:
    void updateWorldState(uint32_t width, uint32_t height);
    
    void updateLights(const simd::float4x4 &modelViewMatrix);
        
    void populateLights();
    
private:
    // Current frame number rendering
    uint64_t m_frameNumber;
    
    int8_t m_frameDataBufferIndex;
    
    // Projection matrix calculated as a function of view size
    simd::float4x4 m_projection_matrix;
    // Projection matrix used to render the shadow map
    simd::float4x4 m_shadowProjectionMatrix;
    
    // Vertex descriptor for models loaded with MetalKit
    MTL::VertexDescriptor m_skyVertexDescriptor;
    
    // Mesh for an icosahedron used for rendering point lights
    MeshPtr m_icosahedronMesh;
    
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
};

}

#endif /* basic_deferred_app_hpp */
