//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef resource_cache_hpp
#define resource_cache_hpp

#include "shader/shader_program.h"
#include "shader/shader_macro_collection.h"
#include "render_pipeline_state.h"
#include <unordered_map>

namespace vox {
class ShaderProgram;

/**
 * @brief Struct to hold the internal state of the Resource Cache
 *
 */
struct ResourceCacheState {
    std::unordered_map<std::size_t, std::shared_ptr<MTL::DepthStencilState>> depthStencilStates;
    
    std::unordered_map<std::size_t, std::unique_ptr<RenderPipelineState>> renderPipelineStates;
    
    std::unordered_map<std::size_t, std::unique_ptr<ShaderProgram>> shaders;
};

/**
 * @brief Cache all sorts of Metal objects specific to a Metal device.
 * Supports serialization and deserialization of cached resources.
 * There is only one cache for all these objects, with several unordered_map of hash indices
 * and objects. For every object requested, there is a templated version on request_resource.
 * Some objects may need building if they are not found in the cache.
 *
 * The resource cache is also linked with ResourceRecord and ResourceReplay. Replay can warm-up
 * the cache on app startup by creating all necessary objects.
 * The cache holds pointers to objects and has a mapping from such pointers to hashes.
 * It can only be destroyed in bulk, single elements cannot be removed.
 */
class ResourceCache {
public:
    ResourceCache(MTL::Device *device);
    
    ResourceCache(const ResourceCache &) = delete;
    
    ResourceCache(ResourceCache &&) = delete;
    
    ResourceCache &operator=(const ResourceCache &) = delete;
    
    ResourceCache &operator=(ResourceCache &&) = delete;
    
public:
    std::shared_ptr<MTL::DepthStencilState>
    requestDepthStencilState(const MTL::DepthStencilDescriptor &descriptor);
    
    RenderPipelineState *
    requestRenderPipelineState(const MTL::RenderPipelineDescriptor &descriptor);
    
    ShaderProgram *requestShader(MTL::Library &library,
                                 const std::string &vertexSource,
                                 const std::string &fragmentSource,
                                 const ShaderMacroCollection &macroInfo);
    
private:
    MTL::Device *_device;
    
    ResourceCacheState _state;
};

}

#endif /* resource_cache_hpp */
