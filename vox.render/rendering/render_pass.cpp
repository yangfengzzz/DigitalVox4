//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_pass.h"
#include "core/cpp_mtl_assert.h"

namespace vox {
RenderPass::RenderPass(MTL::Device* device, MTL::RenderPassDescriptor* desc):
_device(device),
_desc(desc) {
    makeShaderLibrary();
}

void RenderPass::draw(MTL::CommandBuffer& commandBuffer,
                      std::optional<std::string> label) {
    assert(!_subpasses.empty() && "Render pipeline should contain at least one sub-pass");
    
    MTL::RenderCommandEncoder encoder = commandBuffer.renderCommandEncoderWithDescriptor(*_desc);
    if (label) {
        encoder.label(label.value().c_str());
    }
    for (size_t i = 0; i < _subpasses.size(); ++i) {
        _activeSubpassIndex = i;
        _subpasses[i]->draw(encoder);
    }
    _activeSubpassIndex = 0;
    encoder.endEncoding();
}

void RenderPass::addSubpass(std::unique_ptr<Subpass> &&subpass) {
    subpass->setRenderPass(this);
    _subpasses.emplace_back(std::move(subpass));
}

std::vector<std::unique_ptr<Subpass>> &RenderPass::subpasses() {
    return _subpasses;
}

std::unique_ptr<Subpass> &RenderPass::activeSubpass() {
    return _subpasses[_activeSubpassIndex];
}

void RenderPass::makeShaderLibrary() {
    CFErrorRef error = nullptr;
    CFURLRef libraryURL = nullptr;

    libraryURL = CFBundleCopyResourceURL( CFBundleGetMainBundle() , CFSTR("vox.shader"), CFSTR("metallib"), nullptr);
    _library = _device->makeLibrary(libraryURL, &error);
    
    MTLAssert(!error, error, "Could not load Metal shader library");
    
    CFRelease(libraryURL);
}

MTL::Library& RenderPass::library() {
    return _library;
}

}
