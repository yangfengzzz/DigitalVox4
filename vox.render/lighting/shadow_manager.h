//
//  shadow_manager.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/22.
//

#ifndef shadow_manager_hpp
#define shadow_manager_hpp

#include "core/cpp_mtl.h"
#include "loader/texture_loader.h"
#include "scene.h"

namespace vox {
class ShadowManager {
public:
    static constexpr uint32_t MAX_SHADOW = 5;

    ShadowManager(MTL::Library& library, Scene* scene);
    
    void draw(MTL::CommandBuffer& commandBuffer);
    
    void drawSpotShadow(MTL::CommandBuffer& commandBuffer);
    
    void drawDirectionShadow(MTL::CommandBuffer& commandBuffer);

    void drawPointShadow(MTL::CommandBuffer& commandBuffer);
    
private:
    MTL::Library& _library;
    Scene* _scene{nullptr};
    
    const int _shadowMapSize = 2000; // resolution
    uint32_t _shadowCount = 0;
    std::vector<MTL::TexturePtr> _shadowMaps{};
    
    TextureLoader _resourceLoader;
};
}

#endif /* shadow_manager_hpp */
