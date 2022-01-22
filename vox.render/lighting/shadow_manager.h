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
#include "rendering/render_pass.h"
#include "rendering/subpasses/shadow_subpass.h"
#include "scene.h"

namespace vox {
class ShadowManager {
public:
    static constexpr uint32_t MAX_SHADOW = 5;

    struct ShadowData {
        /**
         * Shadow bias.
         */
        float bias = 0.005;
        /**
         * Shadow intensity, the larger the value, the clearer and darker the shadow.
         */
        float intensity = 0.2;
        /**
         * Pixel range used for shadow PCF interpolation.
         */
        float radius = 1;
        /**
         * Light view projection matrix.(cascade)
         */
        Matrix4x4F vp[4];
        /**
         * Light cascade depth.
         */
        float cascadeSplits[4];
    };
    
public:
    ShadowManager(MTL::Library& library, Scene* scene);
    
    void draw(MTL::CommandBuffer& commandBuffer);
    
    void drawSpotShadow(MTL::CommandBuffer& commandBuffer);
    
    void drawDirectionShadow(MTL::CommandBuffer& commandBuffer);

    void drawPointShadow(MTL::CommandBuffer& commandBuffer);
    
private:
    void updateSpotShadowMatrix(SpotLight* light, ShadowManager::ShadowData& shadowData);
    
private:
    MTL::Library& _library;
    Scene* _scene{nullptr};
    
    MTL::RenderPassDescriptor _renderPassDescriptor;
    std::unique_ptr<RenderPass> _renderPass{nullptr};
    ShadowSubpass* _shadowSubpass{nullptr};
    
    const int _shadowMapSize = 2000; // resolution
    uint32_t _shadowCount = 0;
    std::vector<MTL::TexturePtr> _shadowMaps{};
    std::array<ShadowManager::ShadowData, MAX_SHADOW> _shadowDatas{};

    TextureLoader _resourceLoader;
};
}

#endif /* shadow_manager_hpp */
