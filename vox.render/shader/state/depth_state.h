//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef depth_state_hpp
#define depth_state_hpp

#include <Metal/Metal.hpp>

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
    void apply(const std::shared_ptr<MTL::RenderPipelineDescriptor>& pipelineDescriptor,
               const std::shared_ptr<MTL::DepthStencilDescriptor>& depthStencilDescriptor,
               const std::shared_ptr<MTL::RenderCommandEncoder>& encoder) {
        platformApply(pipelineDescriptor, depthStencilDescriptor, encoder);
    }
    
    void platformApply(const std::shared_ptr<MTL::RenderPipelineDescriptor>& pipelineDescriptor,
                       const std::shared_ptr<MTL::DepthStencilDescriptor>& depthStencilDescriptor,
                       const std::shared_ptr<MTL::RenderCommandEncoder>& encoder);
};

}

#endif /* depth_state_hpp */
