//
//  raster_state.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/27.
//

#ifndef raster_state_hpp
#define raster_state_hpp

#include "core/cpp_mtl_DepthStencil.hpp"
#include "core/cpp_mtl_RenderCommandEncoder.hpp"

namespace vox {
/**
 * Raster state.
 */
struct RasterState {
    /** Specifies whether or not front- and/or back-facing polygons can be culled. */
    MTL::CullMode cullMode = MTL::CullModeFront;
    /** The multiplier by which an implementation-specific value is multiplied with to create a constant depth offset. */
    float depthBias = 0;
    /** The scale factor for the variable depth offset for each polygon. */
    float slopeScaledDepthBias = 0;
    
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

#endif /* raster_state_hpp */
