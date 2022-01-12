//
//  depth_state.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/27.
//

#include "depth_state.h"

namespace vox {
void DepthState::platformApply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
                               MTL::DepthStencilDescriptor &depthStencilDescriptor,
                               MTL::RenderCommandEncoder &encoder) {
    if (enabled) {
        // apply compare func.
        depthStencilDescriptor.depthCompareFunction(compareFunction);
        
        // apply write enabled.
        depthStencilDescriptor.depthWriteEnabled(writeEnabled);
    }
}
}
