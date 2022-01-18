//
//  resource_cache.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "resource_cache.h"
#include "CPPMetal.hpp"
#include "std_helpers.h"
#include <utility>

using namespace MTL;

namespace std {
using namespace vox;

template<>
struct hash<RenderPipelineDescriptor> {
    std::size_t operator()(const RenderPipelineDescriptor &descriptor) const {
        std::size_t result = 0;
        
        hash_combine(result, descriptor.vertexDescriptor()->objCObj()); // internal address
        hash_combine(result, descriptor.vertexFunction()->objCObj()); // internal address
        hash_combine(result, descriptor.fragmentFunction()->objCObj()); // internal address
        hash_combine(result, descriptor.sampleCount());
        hash_combine(result, descriptor.isAlphaToCoverageEnabled());
        hash_combine(result, descriptor.depthAttachmentPixelFormat());
        hash_combine(result, descriptor.stencilAttachmentPixelFormat());
        if (descriptor.colorAttachments[0].pixelFormat() != MTL::PixelFormatInvalid) {
            hash_combine(result, descriptor.colorAttachments[0].pixelFormat());
            hash_combine(result, descriptor.colorAttachments[0].isBlendingEnabled());
            hash_combine(result, descriptor.colorAttachments[0].sourceRGBBlendFactor());
            hash_combine(result, descriptor.colorAttachments[0].destinationRGBBlendFactor());
            hash_combine(result, descriptor.colorAttachments[0].rgbBlendOperation());
            hash_combine(result, descriptor.colorAttachments[0].sourceAlphaBlendFactor());
            hash_combine(result, descriptor.colorAttachments[0].destinationAlphaBlendFactor());
            hash_combine(result, descriptor.colorAttachments[0].alphaBlendOperation());
            hash_combine(result, descriptor.colorAttachments[0].writeMask());
        }
        
        return result;
    }
};

template<>
struct hash<DepthStencilDescriptor> {
    std::size_t operator()(const DepthStencilDescriptor &descriptor) const {
        std::size_t result = 0;
        
        hash_combine(result, descriptor.depthCompareFunction());
        hash_combine(result, descriptor.isDepthWriteEnabled());
        hash_combine(result, descriptor.frontFaceStencil.stencilCompareFunction());
        hash_combine(result, descriptor.frontFaceStencil.stencilFailureOperation());
        hash_combine(result, descriptor.frontFaceStencil.depthFailureOperation());
        hash_combine(result, descriptor.frontFaceStencil.depthStencilPassOperation());
        hash_combine(result, descriptor.frontFaceStencil.readMask());
        hash_combine(result, descriptor.frontFaceStencil.writeMask());
        hash_combine(result, descriptor.backFaceStencil.stencilCompareFunction());
        hash_combine(result, descriptor.backFaceStencil.stencilFailureOperation());
        hash_combine(result, descriptor.backFaceStencil.depthFailureOperation());
        hash_combine(result, descriptor.backFaceStencil.depthStencilPassOperation());
        hash_combine(result, descriptor.backFaceStencil.readMask());
        hash_combine(result, descriptor.backFaceStencil.writeMask());
        
        return result;
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
