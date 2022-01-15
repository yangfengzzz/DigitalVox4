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
void LightingSubpass::loadMetal(MTL::VertexDescriptor& m_skyVertexDescriptor) {
    // Create and load the basic Metal state objects
    CFErrorRef error = nullptr;
    
    printf("Selected Device: %s\n", m_view->device().name());
        
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
    
#pragma mark Post lighting depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Less -Writes");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLess);
        depthStencilDesc.depthWriteEnabled(false);
        
        m_dontWriteDepthStencilState = m_device.newDepthStencilStateWithDescriptor(depthStencilDesc);
    }
}

#pragma mark Common Rendering Code
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

}
