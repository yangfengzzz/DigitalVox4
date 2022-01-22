//
//  shadow_manager.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/22.
//

#include "shadow_manager.h"

namespace vox {
ShadowManager::ShadowManager(MTL::Library& library, Scene* scene):
_library(library),
_scene(scene),
_resourceLoader(*scene->device()) {
    scene->registerVertexUploader<std::array<ShadowData, MAX_SHADOW>>([](const std::array<ShadowData, MAX_SHADOW> &x,
                                                                         size_t location, MTL::RenderCommandEncoder& encoder) {
        encoder.setVertexBytes(&x, sizeof(std::array<ShadowData, MAX_SHADOW>), location);
    });
    
    scene->registerFragmentUploader<std::array<ShadowData, MAX_SHADOW>>([](const std::array<ShadowData, MAX_SHADOW> &x,
                                                                           size_t location, MTL::RenderCommandEncoder& encoder) {
        encoder.setFragmentBytes(&x, sizeof(std::array<ShadowData, MAX_SHADOW>), location);
    });
    
    scene->registerVertexUploader<std::array<CubeShadowData, MAX_SHADOW>>([](const std::array<CubeShadowData, MAX_SHADOW> &x,
                                                                             size_t location, MTL::RenderCommandEncoder& encoder) {
        encoder.setVertexBytes(&x, sizeof(std::array<CubeShadowData, MAX_SHADOW>), location);
    });
    
    scene->registerFragmentUploader<std::array<CubeShadowData, MAX_SHADOW>>([](const std::array<CubeShadowData, MAX_SHADOW> &x,
                                                                               size_t location, MTL::RenderCommandEncoder& encoder) {
        encoder.setFragmentBytes(&x, sizeof(std::array<CubeShadowData, MAX_SHADOW>), location);
    });
}

void ShadowManager::draw(MTL::CommandBuffer& commandBuffer) {
    
}

void ShadowManager::drawSpotShadow(MTL::CommandBuffer& commandBuffer) {
    
}

void ShadowManager::drawDirectionShadow(MTL::CommandBuffer& commandBuffer) {
    
}

void ShadowManager::drawPointShadow(MTL::CommandBuffer& commandBuffer) {
    
}

}
