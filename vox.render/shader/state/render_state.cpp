//
//  render_state.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/27.
//

#include "render_state.h"
#include "engine.h"

namespace vox {
void RenderState::apply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
                         MTL::DepthStencilDescriptor &depthStencilDescriptor,
                         MTL::RenderCommandEncoder &encoder) {
    blendState.apply(pipelineDescriptor, depthStencilDescriptor, encoder);
    depthState.apply(pipelineDescriptor, depthStencilDescriptor, encoder);
    stencilState.apply(pipelineDescriptor, depthStencilDescriptor, encoder);
    rasterState.apply(pipelineDescriptor, depthStencilDescriptor, encoder);
}
}
