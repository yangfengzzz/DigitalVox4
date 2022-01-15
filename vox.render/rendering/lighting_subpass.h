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
    
#pragma mark -
    virtual void loadMetal(MTL::VertexDescriptor& m_skyVertexDescriptor);
    
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
        
    MTL::Device m_device;
    
    MTL::View* m_view;
    
    MTL::DepthStencilState *m_dontWriteDepthStencilState;
    
#pragma mark -    
    // Pipeline states
    MTL::RenderPipelineState m_fairyPipelineState;
    MTL::RenderPipelineState m_skyboxPipelineState;
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

}

#endif // AAPLLightingSubpass_h
