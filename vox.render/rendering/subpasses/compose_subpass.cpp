//
//  compose_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "compose_subpass.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
ComposeSubpass::ComposeSubpass(MTL::RenderPassDescriptor* desc,
                               sg::Scene* scene,
                               MTL::Library& shaderLibrary,
                               MTL::Device& m_device,
                               MTL::Buffer &m_quadVertexBuffer):
Subpass(desc, scene),
m_quadVertexBuffer(m_quadVertexBuffer) {
    
}

void ComposeSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    commandEncoder.pushDebugGroup("Draw Directional Light");
    commandEncoder.setFragmentTexture(gbuffer_desc->colorAttachments[RenderTargetAlbedo].texture(), RenderTargetAlbedo);
    commandEncoder.setFragmentTexture(gbuffer_desc->colorAttachments[RenderTargetNormal].texture(), RenderTargetNormal);
    commandEncoder.setFragmentTexture(gbuffer_desc->colorAttachments[RenderTargetDepth].texture(), RenderTargetDepth);
    
    commandEncoder.setCullMode(MTL::CullModeBack);
    commandEncoder.setStencilReferenceValue(128);
    
    commandEncoder.setRenderPipelineState(m_directionalLightPipelineState);
    commandEncoder.setDepthStencilState(m_directionLightDepthStencilState);
    commandEncoder.setVertexBuffer(m_quadVertexBuffer, 0, BufferIndexMeshPositions);
    commandEncoder.setVertexBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    commandEncoder.setFragmentBuffer(std::any_cast<MTL::Buffer>(scene->shaderData.getData("frameData")), 0, BufferIndexFrameData);
    
    // Draw full screen quad
    commandEncoder.drawPrimitives(MTL::PrimitiveTypeTriangle, 0, 6);
    commandEncoder.popDebugGroup();
}

}
