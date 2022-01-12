//
//  raster_state.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/27.
//

#include "raster_state.h"

namespace vox {
void RasterState::platformApply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
                                MTL::DepthStencilDescriptor &depthStencilDescriptor,
                                MTL::RenderCommandEncoder &encoder) {
    bool cullFaceEnable = cullMode != MTL::CullModeNone;
    
    // apply front face.
    if (cullFaceEnable) {
        encoder.setCullMode(cullMode);
    }
    
    // apply polygonOffset.
    if (depthBias != 0 || slopeScaledDepthBias != 0) {
        encoder.setDepthBias(depthBias, slopeScaledDepthBias, 0);
    }
}

}
