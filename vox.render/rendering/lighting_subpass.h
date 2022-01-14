/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Header for subpass class which performs Metal setup and per frame rendering
 */
#ifndef AAPLLightingSubpass_h
#define AAPLLightingSubpass_h

#include "config.h"
#include "mesh.h"
#include "math_utilities.h"

#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>

#include "CPPMetal.hpp"

namespace vox {
// Number of "fairy" lights in scene
static const uint32_t NumLights = 256;

class LightingSubpass {
public:
    explicit LightingSubpass(MTL::View *view);
    
    virtual ~LightingSubpass();
    
    MTL::Device device() const;
    
    MTL::View *view();
    
    MTL::PixelFormat colorTargetPixelFormat() const;
    
    MTL::PixelFormat depthStencilTargetPixelFormat() const;
    
    MTL::Texture &depthStencilTexture();
    
    MTL::DepthStencilState &pointLightDepthStencilState();
    
    MTL::Texture &albedo_specular_GBuffer();
    
    MTL::Texture &normal_shadow_GBuffer();
    
    MTL::Texture &depth_GBuffer();
    
    // Open the Metal shader library
    MTL::Library makeShaderLibrary();
    
#pragma mark -
    virtual void loadMetal(MTL::VertexDescriptor& m_defaultVertexDescriptor,
                           MTL::VertexDescriptor& m_skyVertexDescriptor);
    
    void endFrame(MTL::CommandBuffer &commandBuffer);
    
    void drawGBuffer(MTL::RenderCommandEncoder &renderEncoder,
                     std::vector<Mesh> *m_meshes,
                     MTL::Buffer& m_uniformBuffer,
                     MTL::Texture& m_shadowMap);
    
    void drawDirectionalLight(MTL::RenderCommandEncoder &renderEncoder,
                              MTL::Buffer& m_quadVertexBuffer,
                              MTL::Buffer& m_uniformBuffer);
    
    void drawPointLightMask(MTL::RenderCommandEncoder &renderEncoder,
                            MTL::Buffer& m_lightsData,
                            MTL::Buffer& m_lightPosition,
                            MTL::Buffer& m_uniformBuffer,
                            Mesh& m_icosahedronMesh);
    
    void drawPointLights(MTL::RenderCommandEncoder &renderEncoder,
                         MTL::Buffer& m_lightsData,
                         MTL::Buffer& m_lightPosition,
                         MTL::Buffer& m_uniformBuffer,
                         Mesh& m_icosahedronMesh);
    
    void drawFairies(MTL::RenderCommandEncoder &renderEncoder,
                     MTL::Buffer& m_lightsData,
                     MTL::Buffer& m_lightPosition,
                     MTL::Buffer& m_uniformBuffer,
                     MTL::Buffer& m_fairy,
                     MTL::Texture& m_fairyMap);
    
    void drawSky(MTL::RenderCommandEncoder &renderEncoder,
                 MTL::Buffer& m_uniformBuffer,
                 Mesh& m_skyMesh,
                 MTL::Texture& m_skyMap);
    
    void drawableSizeWillChange(MTL::Size size, MTL::StorageMode GBufferStorageMode);
    
    MTL::Device m_device;
    
    MTL::View* m_view;
    
    // GBuffer properties
    
    MTL::PixelFormat m_albedo_specular_GBufferFormat;
    
    MTL::PixelFormat m_normal_shadow_GBufferFormat;
    
    MTL::PixelFormat m_depth_GBufferFormat;
    
    MTL::Texture m_albedo_specular_GBuffer;
    
    MTL::Texture m_normal_shadow_GBuffer;
    
    MTL::Texture m_depth_GBuffer;
    
    MTL::DepthStencilState *m_dontWriteDepthStencilState;
    
#pragma mark -
    void drawMeshes(MTL::RenderCommandEncoder &renderEncoder, std::vector<Mesh> *m_meshes);
    
    // Pipeline states
    MTL::RenderPipelineState m_GBufferPipelineState;
    MTL::RenderPipelineState m_fairyPipelineState;
    MTL::RenderPipelineState m_skyboxPipelineState;
    MTL::RenderPipelineState m_directionalLightPipelineState;
    
    MTL::RenderPipelineState m_lightPipelineState;
    
    // Depht Stencitl States
    MTL::DepthStencilState m_directionLightDepthStencilState;
    MTL::DepthStencilState m_GBufferDepthStencilState;
    MTL::DepthStencilState m_pointLightDepthStencilState;
    
#if LIGHT_STENCIL_CULLING
    MTL::RenderPipelineState m_lightMaskPipelineState;
    MTL::DepthStencilState m_lightMaskDepthStencilState;
#endif
};


inline MTL::Device LightingSubpass::device() const {
    return m_device;
}

inline MTL::View* LightingSubpass::view() {
    return m_view;
}

inline MTL::PixelFormat LightingSubpass::colorTargetPixelFormat() const {
    return m_view->colorPixelFormat();
}

inline MTL::PixelFormat LightingSubpass::depthStencilTargetPixelFormat() const {
    return m_view->depthStencilPixelFormat();
}

inline MTL::Texture &LightingSubpass::depthStencilTexture() {
    return *(m_view->depthStencilTexture());
}

inline MTL::DepthStencilState &LightingSubpass::pointLightDepthStencilState() {
    return m_pointLightDepthStencilState;
}

inline MTL::Texture &LightingSubpass::albedo_specular_GBuffer() {
    return m_albedo_specular_GBuffer;
}

inline MTL::Texture &LightingSubpass::normal_shadow_GBuffer() {
    return m_normal_shadow_GBuffer;
}

inline MTL::Texture &LightingSubpass::depth_GBuffer() {
    return m_depth_GBuffer;
}

}

#endif // AAPLLightingSubpass_h