/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Header for subpass class which performs Metal setup and per frame rendering
 */
#ifndef AAPLSubpass_h
#define AAPLSubpass_h

#include "config.h"
#include "mesh.h"
#include "math_utilities.h"

#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>

#include "CPPMetal.hpp"

namespace vox {
// The max number of command buffers in flight
static const uint8_t MaxFramesInFlight = 3;

// Number of "fairy" lights in scene
static const uint32_t NumLights = 256;

static const float NearPlane = 1;
static const float FarPlane = 150;

class Subpass {
public:
    explicit Subpass(MTL::View *view);
    
    virtual ~Subpass();
    
    MTL::Device device() const;
    
    MTL::View *view();
    
    const Mesh &icosahedronMesh() const;
    
    MTL::PixelFormat colorTargetPixelFormat() const;
    
    MTL::PixelFormat depthStencilTargetPixelFormat() const;
    
    const MTL::Buffer &quadVertexBuffer() const;
    
    MTL::Texture &depthStencilTexture();
    
    int8_t frameDataBufferIndex() const;
    
    MTL::Buffer &frameDataBuffer(int8_t frameDataBufferIndex);
    
    MTL::Buffer &lightPositions(int8_t frameDataBufferIndex);
    
    MTL::Buffer &lightsData();
    
    MTL::DepthStencilState &pointLightDepthStencilState();
    
    MTL::Texture &albedo_specular_GBuffer();
    
    MTL::Texture &normal_shadow_GBuffer();
    
    MTL::Texture &depth_GBuffer();
    
    MTL::Texture &shadowMap();
    
    // Open the Metal shader library
    MTL::Library makeShaderLibrary();
    
#pragma mark -
    virtual void loadMetal();
    
    void loadScene();
    
    MTL::CommandBuffer beginFrame();
    
    MTL::CommandBuffer beginDrawableCommands();
    
    void endFrame(MTL::CommandBuffer &commandBuffer);
    
    void drawShadow(MTL::CommandBuffer &commandBuffer);
    
    void drawGBuffer(MTL::RenderCommandEncoder &renderEncoder);
    
    void drawDirectionalLightCommon(MTL::RenderCommandEncoder &renderEncoder);
    
    void drawPointLightMask(MTL::RenderCommandEncoder &renderEncoder);
    
    void drawPointLightsCommon(MTL::RenderCommandEncoder &renderEncoder);
    
    void drawFairies(MTL::RenderCommandEncoder &renderEncoder);
    
    void drawSky(MTL::RenderCommandEncoder &renderEncoder);
    
    void drawableSizeWillChange(MTL::Size size, MTL::StorageMode GBufferStorageMode);
    
    MTL::Texture *currentDrawableTexture();
    
    MTL::Device m_device;
    
    MTL::View* m_view;
    
    int8_t m_frameDataBufferIndex;
    
    // GBuffer properties
    
    MTL::PixelFormat m_albedo_specular_GBufferFormat;
    
    MTL::PixelFormat m_normal_shadow_GBufferFormat;
    
    MTL::PixelFormat m_depth_GBufferFormat;
    
    MTL::Texture m_albedo_specular_GBuffer;
    
    MTL::Texture m_normal_shadow_GBuffer;
    
    MTL::Texture m_depth_GBuffer;
    
    MTL::DepthStencilState *m_dontWriteDepthStencilState;
    
#pragma mark -
    void updateLights(const simd::float4x4 &modelViewMatrix);
    
    void updateWorldState();
    
    void drawMeshes(MTL::RenderCommandEncoder &renderEncoder);
    
    dispatch_semaphore_t m_inFlightSemaphore;
    
    MTL::CommandBufferHandler *m_completedHandler;
    
    // Vertex descriptor for models loaded with MetalKit
    MTL::VertexDescriptor m_defaultVertexDescriptor;
    
    MTL::CommandQueue m_commandQueue;
    
    // Pipeline states
    MTL::RenderPipelineState m_GBufferPipelineState;
    MTL::RenderPipelineState m_fairyPipelineState;
    MTL::RenderPipelineState m_skyboxPipelineState;
    MTL::RenderPipelineState m_shadowGenPipelineState;
    MTL::RenderPipelineState m_directionalLightPipelineState;
    
    // Depht Stencitl States
    MTL::DepthStencilState m_directionLightDepthStencilState;
    MTL::DepthStencilState m_GBufferDepthStencilState;
    MTL::DepthStencilState m_shadowDepthStencilState;
    MTL::DepthStencilState m_pointLightDepthStencilState;
    
#if LIGHT_STENCIL_CULLING
    MTL::RenderPipelineState m_lightMaskPipelineState;
    MTL::DepthStencilState m_lightMaskDepthStencilState;
#endif
    
    MTL::RenderPassDescriptor m_shadowRenderPassDescriptor;
    
    // Depth render target for shadow map
    MTL::Texture m_shadowMap;
    
    // Texture to create smooth round particles
    MTL::Texture m_fairyMap;
    
    // Texture for skybox
    MTL::Texture m_skyMap;
    
    // Buffers used to store dynamically changing per frame data
    MTL::Buffer m_uniformBuffers[MaxFramesInFlight];
    
    // Buffers used to story dynamically changing light positions
    MTL::Buffer m_lightPositions[MaxFramesInFlight];
    
    // Buffer for constant light data
    MTL::Buffer m_lightsData;
    
    // Mesh buffer for simple Quad
    MTL::Buffer m_quadVertexBuffer;
    
    // Mesh buffer for fairies
    MTL::Buffer m_fairy;
    
    // Array of meshes loaded from the model file
    std::vector<Mesh> *m_meshes;
    
    // Mesh for sphere use to render the skybox
    Mesh m_skyMesh;
    
    // Projection matrix calculated as a function of view size
    simd::float4x4 m_projection_matrix;
    
    // Projection matrix used to render the shadow map
    simd::float4x4 m_shadowProjectionMatrix;
    
    // Current frame number rendering
    uint64_t m_frameNumber;
    
    // Vertex descriptor for models loaded with MetalKit
    MTL::VertexDescriptor m_skyVertexDescriptor;
    
    // Light positions before transformation to positions in current frame
    simd::float4 *m_originalLightPositions;
    // Mesh for an icosahedron used for rendering point lights
    Mesh m_icosahedronMesh;
    
    void populateLights();
};


inline MTL::Device Subpass::device() const {
    return m_device;
}

inline MTL::View* Subpass::view() {
    return m_view;
}

inline const Mesh &Subpass::icosahedronMesh() const {
    return m_icosahedronMesh;
}

inline MTL::PixelFormat Subpass::colorTargetPixelFormat() const {
    return m_view->colorPixelFormat();
}

inline MTL::PixelFormat Subpass::depthStencilTargetPixelFormat() const {
    return m_view->depthStencilPixelFormat();
}

inline const MTL::Buffer &Subpass::quadVertexBuffer() const {
    return m_quadVertexBuffer;
}

inline MTL::Texture &Subpass::depthStencilTexture() {
    return *(m_view->depthStencilTexture());
}

inline int8_t Subpass::frameDataBufferIndex() const {
    return m_frameDataBufferIndex;
}

inline MTL::Buffer &Subpass::frameDataBuffer(int8_t frameDataBufferIndex) {
    return m_uniformBuffers[frameDataBufferIndex];
}

inline MTL::Buffer &Subpass::lightPositions(int8_t frameDataBufferIndex) {
    return m_lightPositions[frameDataBufferIndex];
}

inline MTL::Buffer &Subpass::lightsData() {
    return m_lightsData;
}

inline MTL::DepthStencilState &Subpass::pointLightDepthStencilState() {
    return m_pointLightDepthStencilState;
}

inline MTL::Texture &Subpass::albedo_specular_GBuffer() {
    return m_albedo_specular_GBuffer;
}

inline MTL::Texture &Subpass::normal_shadow_GBuffer() {
    return m_normal_shadow_GBuffer;
}

inline MTL::Texture &Subpass::depth_GBuffer() {
    return m_depth_GBuffer;
}

inline MTL::Texture &Subpass::shadowMap() {
    return m_shadowMap;
}

}

#endif // AAPLSubpass_h
