//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "resource_cache.h"
#include "cpp_mtl.h"
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
        if (descriptor.fragmentFunction()) {
            hash_combine(result, descriptor.fragmentFunction()->objCObj()); // internal address
        }
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
ResourceCache::ResourceCache(Device *device) :
_device{device} {
}

MTL::RenderPipelineState *ResourceCache::requestRenderPipelineState(MTL::RenderPipelineDescriptor &descriptor) {
    std::hash<RenderPipelineDescriptor> hasher;
    size_t hash = hasher(descriptor);
    
    auto iter = _state.renderPipelineStates.find(hash);
    if (iter == _state.renderPipelineStates.end()) {
        auto pipelineState = std::unique_ptr<MTL::RenderPipelineState>(_device->newRenderPipelineStateWithDescriptor(descriptor));
        _state.renderPipelineStates[hash] = std::move(pipelineState);
        return _state.renderPipelineStates[hash].get();
    } else {
        return iter->second.get();
    }
}

MTL::DepthStencilState *ResourceCache::requestDepthStencilState(MTL::DepthStencilDescriptor &descriptor) {
    std::hash<DepthStencilDescriptor> hasher;
    size_t hash = hasher(descriptor);
    
    auto iter = _state.depthStencilStates.find(hash);
    if (iter == _state.depthStencilStates.end()) {
        auto depthStencilState = std::unique_ptr<MTL::DepthStencilState>(_device->newDepthStencilStateWithDescriptor(descriptor));
        _state.depthStencilStates[hash] = std::move(depthStencilState);
        return _state.depthStencilStates[hash].get();
    } else {
        return iter->second.get();
    }
}

ShaderProgram *ResourceCache::requestShader(MTL::Library& library,
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
