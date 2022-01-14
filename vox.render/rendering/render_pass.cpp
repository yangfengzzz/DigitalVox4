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

void RenderPass::draw(MTL::CommandBuffer& commandBuffer,
                      std::optional<std::string> label) {
    assert(!subpasses.empty() && "Render pipeline should contain at least one sub-pass");
    
    MTL::RenderCommandEncoder encoder = commandBuffer.renderCommandEncoderWithDescriptor(*desc);
    if (label) {
        encoder.label(label.value().c_str());
    }
    for (size_t i = 0; i < subpasses.size(); ++i) {
        active_subpass_index = i;
        subpasses[i]->draw(encoder);
    }
    active_subpass_index = 0;
    encoder.endEncoding();
}

void RenderPass::addSubpass(std::unique_ptr<Subpass> &&subpass) {
    subpasses.emplace_back(std::move(subpass));
}

std::vector<std::unique_ptr<Subpass>> &RenderPass::getSubpasses() {
    return subpasses;
}

std::unique_ptr<Subpass> &RenderPass::getActiveSubpass() {
    return subpasses[active_subpass_index];
}

}
