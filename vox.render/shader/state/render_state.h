//
//  render_state.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/27.
//

#ifndef render_state_hpp
#define render_state_hpp

#include "blend_state.h"
#include "raster_state.h"
#include "depth_state.h"
#include "stencil_state.h"

namespace vox {
/**
 * Render state.
 */
struct RenderState {
    /** Blend state. */
    BlendState blendState = BlendState();
    /** Depth state. */
    DepthState depthState = DepthState();
    /** Stencil state. */
    StencilState stencilState = StencilState();
    /** Raster state. */
    RasterState rasterState = RasterState();
    
    void apply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
               MTL::DepthStencilDescriptor &depthStencilDescriptor,
               MTL::RenderCommandEncoder &encoder);
};

}

#endif /* render_state_hpp */
