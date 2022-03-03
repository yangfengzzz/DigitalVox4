//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_pass.h"
#include "render_subpass.h"
#include "compute_subpass.h"
#include "metal_helpers.h"
#include <glog/logging.h>

namespace vox {
RenderPass::RenderPass(MTL::Library &library,
                       MTL::RenderPassDescriptor &desc) :
_library(library),
_desc(desc),
_resourceCache(library.device()) {
}

const MTL::RenderPassDescriptor &RenderPass::renderPassDescriptor() const {
    return _desc;
}

MTL::Library &RenderPass::library() {
    return _library;
}

void RenderPass::setGUI(GUI *gui) {
    _gui = gui;
}

//MARK: - Subpass
void RenderPass::draw(MTL::CommandBuffer &commandBuffer,
                      std::optional<std::string> label) {
    assert(!_subpasses.empty() && "Render pipeline should contain at least one sub-pass");
    
    std::shared_ptr<NS::String> l{nullptr};
    if (label) {
        l = CLONE_METAL_CUSTOM_DELETER(NS::String, NS::String::string(label.value().c_str(), NS::StringEncoding::UTF8StringEncoding));
    }
    
    std::pair<Subpass::Type, std::shared_ptr<MTL::CommandEncoder>> preEncoder;
    auto encoderCreator = [&](Subpass* subpass) {
        if (subpass->type() == Subpass::Type::Render) {
            std::shared_ptr<MTL::RenderCommandEncoder> encoder = CLONE_METAL_CUSTOM_DELETER(MTL::RenderCommandEncoder,
                                                                                            commandBuffer.renderCommandEncoder(&_desc));
            if (l) {
                encoder->setLabel(l.get());
            }
            static_cast<RenderSubpass*>(subpass)->draw(*encoder);
            
            preEncoder.first = Subpass::Type::Render;
            preEncoder.second = encoder;
        } else {
            std::shared_ptr<MTL::ComputeCommandEncoder> encoder = CLONE_METAL_CUSTOM_DELETER(MTL::ComputeCommandEncoder,
                                                                                             commandBuffer.computeCommandEncoder());
            if (l) {
                encoder->setLabel(l.get());
            }
            static_cast<ComputeSubpass*>(subpass)->compute(*encoder);
            
            preEncoder.first = Subpass::Type::Compute;
            preEncoder.second = encoder;
        }
    };
    
    for (size_t i = 0; i < _subpasses.size(); ++i) {
        _activeSubpassIndex = i;
        auto subpass = _subpasses[i].get();
        if (i == 0) {
            encoderCreator(subpass);
            continue;
        }
        
        if (subpass->type() == preEncoder.first) {
            if (subpass->type() == Subpass::Type::Render) {
                static_cast<RenderSubpass*>(subpass)->draw(*static_cast<MTL::RenderCommandEncoder*>(preEncoder.second.get()));
            } else {
                static_cast<ComputeSubpass*>(subpass)->compute(*static_cast<MTL::ComputeCommandEncoder*>(preEncoder.second.get()));
            }
        } else {
            preEncoder.second->endEncoding();
            encoderCreator(subpass);
        }
    }
    _activeSubpassIndex = 0;
    
    if (_gui) {
        ImDrawData *drawData = ImGui::GetDrawData();
        if (drawData) {
            if (preEncoder.first != Subpass::Type::Render) {
                preEncoder.second->endEncoding();
                std::shared_ptr<MTL::RenderCommandEncoder> encoder = CLONE_METAL_CUSTOM_DELETER(MTL::RenderCommandEncoder,
                                                                                                commandBuffer.renderCommandEncoder(&_desc));
                if (l) {
                    encoder->setLabel(l.get());
                }
                preEncoder.first = Subpass::Type::Render;
                preEncoder.second = encoder;
            }
            
            auto guiLabel = CLONE_METAL_CUSTOM_DELETER(NS::String, NS::String::string("GUI Rendering", NS::StringEncoding::UTF8StringEncoding));
            preEncoder.second->pushDebugGroup(guiLabel.get());
            _gui->newFrame(_desc);
            _gui->draw(drawData, commandBuffer, *static_cast<MTL::RenderCommandEncoder*>(preEncoder.second.get()));
            preEncoder.second->popDebugGroup();
        }
    }
    preEncoder.second->endEncoding();
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

//MARK: - ParentPass
void RenderPass::addParentPass(const std::string &name, RenderPass *pass) {
    auto iter = _parentPass.find(name);
    if (iter == _parentPass.end()) {
        _parentPass[name] = pass;
    } else {
        LOG(WARNING) << "already exists\n";
    }
}

RenderPass *RenderPass::removeParentPass(const std::string &name) {
    auto iter = _parentPass.find(name);
    if (iter != _parentPass.end()) {
        auto pass = iter->second;
        _parentPass.erase(iter);
        return pass;
    } else {
        LOG(WARNING) << "can't find pass\n";
        return nullptr;
    }
}

RenderPass *RenderPass::findPass(const std::string &name) {
    auto iter = _parentPass.find(name);
    if (iter != _parentPass.end()) {
        return iter->second;
    } else {
        return nullptr;
    }
}

void RenderPass::clearParentPass() {
    _parentPass.clear();
}

ResourceCache &RenderPass::resourceCache() {
    return _resourceCache;
}

}
