//
//  depth_state.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/27.
//

#ifndef depth_state_hpp
#define depth_state_hpp

#include "core/CPPMetalDepthStencil.hpp"
#include "core/CPPMetalRenderCommandEncoder.hpp"

namespace vox {
/**
 * Depth state.
 */
struct DepthState {
    /** Whether to enable the depth test. */
    bool enabled = true;
    /** Whether the depth value can be written.*/
    bool writeEnabled = true;
    /** Depth comparison function. */
    MTL::CompareFunction compareFunction = MTL::CompareFunctionLess;
    
    /**
     * Apply the current depth state by comparing with the last depth state.
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

#endif /* depth_state_hpp */
