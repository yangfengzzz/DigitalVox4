//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "resource_cache.h"
#include <Metal/Metal.hpp>
#include "std_helpers.h"
#include "metal_helpers.h"

//MARK: - ResourceCache
namespace vox {
ResourceCache::ResourceCache(MTL::Device *device) :
_device{device} {
}

std::shared_ptr<MTL::DepthStencilState>
ResourceCache::requestDepthStencilState(const MTL::DepthStencilDescriptor &descriptor) {
    size_t hash = descriptor.hash();
    
    auto iter = _state.depthStencilStates.find(hash);
    if (iter == _state.depthStencilStates.end()) {
        auto depthStencilState = CLONE_METAL_CUSTOM_DELETER(MTL::DepthStencilState,
                                                            _device->newDepthStencilState(&descriptor));
        _state.depthStencilStates[hash] = std::move(depthStencilState);
        return _state.depthStencilStates[hash];
    } else {
        return iter->second;
    }
}

RenderPipelineState *
ResourceCache::requestRenderPipelineState(const MTL::RenderPipelineDescriptor &descriptor) {
    size_t hash = descriptor.hash();
    
    auto iter = _state.renderPipelineStates.find(hash);
    if (iter == _state.renderPipelineStates.end()) {
        auto pipelineState = std::make_unique<RenderPipelineState>(_device, descriptor);
        _state.renderPipelineStates[hash] = std::move(pipelineState);
        return _state.renderPipelineStates[hash].get();
    } else {
        return iter->second.get();
    }
}

ShaderProgram *ResourceCache::requestShader(MTL::Library &library,
                                            const std::string &vertexSource,
                                            const std::string &fragmentSource,
                                            const ShaderMacroCollection &macroInfo) {
    std::size_t hash{0U};
    hash_combine(hash, std::hash<std::string>{}(vertexSource));
    hash_combine(hash, std::hash<std::string>{}(fragmentSource));
    hash_combine(hash, macroInfo.hash());
    
    auto iter = _state.shaders.find(hash);
    if (iter == _state.shaders.end()) {
        auto shader = std::make_unique<ShaderProgram>(library, vertexSource, fragmentSource, macroInfo);
        _state.shaders[hash] = std::move(shader);
        return _state.shaders[hash].get();
    } else {
        return iter->second.get();
    }
}

}
