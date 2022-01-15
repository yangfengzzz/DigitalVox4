//
//  resource_cache.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "resource_cache.h"
#include <utility>

using namespace MTL;

namespace std {
template<>
struct hash<RenderPipelineDescriptor> {
    std::size_t operator()(const RenderPipelineDescriptor &descriptor) const {
        return 0;
    }
};

template<>
struct hash<DepthStencilDescriptor> {
    std::size_t operator()(const DepthStencilDescriptor &descriptor) const {
        return 0;
    }
};

template<>
struct hash<Function> {
    std::size_t operator()(const std::string &name, const vox::ShaderMacroCollection &macroInfo) const {
        return 0;
    }
};

}

//MARK: - ResourceCache
namespace vox {
ResourceCache::ResourceCache(Device &device) :
device{device} {
}

MTL::RenderPipelineState &ResourceCache::requestRenderPipelineState(MTL::RenderPipelineDescriptor &descriptor) {
    std::hash<RenderPipelineDescriptor> hasher;
    size_t hash = hasher(descriptor);

    auto iter = state.renderPipelineStates.find(hash);
    if (iter == state.renderPipelineStates.end()) {
        auto pipelineState = device.makeRenderPipelineState(descriptor);
        state.renderPipelineStates[hash] = std::move(pipelineState);
        return state.renderPipelineStates[hash];
    } else {
        return iter->second;
    }
}

MTL::DepthStencilState &ResourceCache::requestDepthStencilState(MTL::DepthStencilDescriptor &descriptor) {
    std::hash<DepthStencilDescriptor> hasher;
    size_t hash = hasher(descriptor);
    
    auto iter = state.depthStencilStates.find(hash);
    if (iter == state.depthStencilStates.end()) {
        auto depthStencilState = device.makeDepthStencilState(descriptor);
        state.depthStencilStates[hash] = std::move(depthStencilState);
        return state.depthStencilStates[hash];
    } else {
        return iter->second;
    }
}

MTL::Function &ResourceCache::requestFunction(const std::string &name, const ShaderMacroCollection &macroInfo) {
    std::hash<Function> hasher;
    size_t hash = hasher(name, macroInfo);
    
    auto iter = state.functions.find(hash);
    if (iter == state.functions.end()) {
//        auto depthStencilState = device.makeDepthStencilState(descriptor);
//        state.functions[hash] = std::move(depthStencilState);
        return state.functions[0];
    } else {
        return iter->second;
    }
}

}
