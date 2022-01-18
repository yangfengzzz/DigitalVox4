//
//  blend_state.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/27.
//

#ifndef blend_state_hpp
#define blend_state_hpp

#include "renderTarget_blend_state.h"
#include "color.h"
#include "core/cpp_mtl_render_command_encoder.h"

namespace vox {
/**
 * Blend state.
 */
struct BlendState {
    /** The blend state of the render target. */
    RenderTargetBlendState targetBlendState = RenderTargetBlendState();
    /** Constant blend color. */
    Color blendColor = Color(0, 0, 0, 0);
    /** Whether to use (Alpha-to-Coverage) technology. */
    bool alphaToCoverage = false;
    
    /**
     * Apply the current blend state by comparing with the last blend state.
     */
    void apply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
               MTL::DepthStencilDescriptor &depthStencilDescriptor,
               MTL::RenderCommandEncoder &encoder) {
        platformApply(pipelineDescriptor, depthStencilDescriptor, encoder);
    }
    
    void platformApply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
                       MTL::DepthStencilDescriptor &depthStencilDescriptor,
                       MTL::RenderCommandEncoder &encoder);
};

}

#endif /* blend_state_hpp */
