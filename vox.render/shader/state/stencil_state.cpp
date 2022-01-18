//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "stencil_state.h"

namespace vox {
void StencilState::platformApply(MTL::RenderPipelineDescriptor& pipelineDescriptor,
                                 MTL::DepthStencilDescriptor& depthStencilDescriptor,
                                 MTL::RenderCommandEncoder &encoder) {
    if (enabled) {
        // apply stencil func.
        depthStencilDescriptor.frontFaceStencil.stencilCompareFunction(compareFunctionFront);
        depthStencilDescriptor.frontFaceStencil.readMask(mask);
        
        depthStencilDescriptor.backFaceStencil.stencilCompareFunction(compareFunctionBack);
        depthStencilDescriptor.backFaceStencil.readMask(mask);
        
        encoder.setStencilReferenceValue(referenceValue);
    }
    
    // apply stencil operation.
    depthStencilDescriptor.frontFaceStencil.stencilFailureOperation(failOperationFront);
    depthStencilDescriptor.frontFaceStencil.depthFailureOperation(zFailOperationFront);
    depthStencilDescriptor.frontFaceStencil.depthStencilPassOperation(passOperationFront);
    
    depthStencilDescriptor.backFaceStencil.stencilFailureOperation(failOperationBack);
    depthStencilDescriptor.backFaceStencil.depthFailureOperation(zFailOperationBack);
    depthStencilDescriptor.backFaceStencil.depthStencilPassOperation(passOperationBack);
    
    // apply write mask.
    depthStencilDescriptor.frontFaceStencil.writeMask(writeMask);
    depthStencilDescriptor.backFaceStencil.writeMask(writeMask);
}
}

