//
//  resource_cache.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "resource_cache.h"

namespace vox {
using namespace MTL;

ResourceCache::ResourceCache(Device &device) :
device{device} {
}

MTL::RenderPipelineState &ResourceCache::requestRenderPipelineState(MTL::RenderPipelineDescriptor &descriptor) {
    return state.renderPipelineStates[0];
}

MTL::DepthStencilState &ResourceCache::requestDepthStencilState(MTL::DepthStencilDescriptor &descriptor) {
    return state.depthStencilStates[0];
}

MTL::Function &ResourceCache::requestFunction(const std::string &name, const ShaderMacroCollection &macroInfo) {
    return state.functions[0];
}

}
