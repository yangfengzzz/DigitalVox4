//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
