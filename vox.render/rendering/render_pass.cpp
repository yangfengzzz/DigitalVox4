//
//  render_pass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "render_pass.h"

namespace vox {
RenderPass::RenderPass(MTL::RenderPassDescriptor* desc):
desc(desc) {
}

void RenderPass::draw(MTL::CommandBuffer commandBuffer,
                      MTL::CommandBufferHandler* handler) {
    assert(!subpasses.empty() && "Render pipeline should contain at least one sub-pass");
    
    MTL::CommandEncoder encoder = commandBuffer.renderCommandEncoderWithDescriptor(*desc);
    for (size_t i = 0; i < subpasses.size(); ++i) {
        active_subpass_index = i;
        subpasses[i]->draw(encoder);
    }
    active_subpass_index = 0;
    
    if (handler) {
        commandBuffer.addScheduledHandler(*handler);
    }
    commandBuffer.commit();
}

}
