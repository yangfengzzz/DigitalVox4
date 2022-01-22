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
    scene->registerVertexUploader<std::array<ShadowManager::ShadowData, MAX_SHADOW>>([](const std::array<ShadowManager::ShadowData, MAX_SHADOW> &x,
                                                                                        size_t location, MTL::RenderCommandEncoder& encoder) {
        encoder.setVertexBytes(&x, sizeof(std::array<ShadowManager::ShadowData, MAX_SHADOW>), location);
    });
    
    scene->registerFragmentUploader<std::array<ShadowManager::ShadowData, MAX_SHADOW>>([](const std::array<ShadowManager::ShadowData, MAX_SHADOW> &x,
                                                                                          size_t location, MTL::RenderCommandEncoder& encoder) {
        encoder.setFragmentBytes(&x, sizeof(std::array<ShadowManager::ShadowData, MAX_SHADOW>), location);
    });
    
    scene->registerVertexUploader<std::array<CubeShadowData, MAX_SHADOW>>([](const std::array<CubeShadowData, MAX_SHADOW> &x,
                                                                             size_t location, MTL::RenderCommandEncoder& encoder) {
        encoder.setVertexBytes(&x, sizeof(std::array<CubeShadowData, MAX_SHADOW>), location);
    });
    
    scene->registerFragmentUploader<std::array<CubeShadowData, MAX_SHADOW>>([](const std::array<CubeShadowData, MAX_SHADOW> &x,
                                                                               size_t location, MTL::RenderCommandEncoder& encoder) {
        encoder.setFragmentBytes(&x, sizeof(std::array<CubeShadowData, MAX_SHADOW>), location);
    });
    
    _renderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
    _renderPass = std::make_unique<RenderPass>(_library, &_renderPassDescriptor);
    auto shadowSubpass = std::make_unique<ShadowSubpass>(nullptr, _scene, nullptr);
    _shadowSubpass = shadowSubpass.get();
    _renderPass->addSubpass(std::move(shadowSubpass));
}

void ShadowManager::draw(MTL::CommandBuffer& commandBuffer) {
    
}

void ShadowManager::drawSpotShadow(MTL::CommandBuffer& commandBuffer) {
    _shadowCount = 0;
    const auto &lights = _scene->light_manager.spotLights();
    for (const auto &light: lights) {
        if (light->enableShadow() && _shadowCount < MAX_SHADOW) {
            MTL::TexturePtr texture = nullptr;
            if (_shadowCount < _shadowMaps.size()) {
                texture = _shadowMaps[_shadowCount];
            } else {
                texture = _resourceLoader.buildTexture(_shadowMapSize, _shadowMapSize,
                                                       MTL::PixelFormatDepth32Float);
                _shadowMaps.push_back(texture);
            }
            _renderPassDescriptor.depthAttachment.texture(*texture);

            updateSpotShadowMatrix(light, _shadowDatas[_shadowCount]);
            _shadowSubpass->setViewProjectionMatrix(_shadowDatas[_shadowCount].vp[0]);
            _renderPass->draw(commandBuffer, "Spot Shadow Pass");
            _shadowCount++;
        }
    }
}

void ShadowManager::drawDirectionShadow(MTL::CommandBuffer& commandBuffer) {
    
}

void ShadowManager::drawPointShadow(MTL::CommandBuffer& commandBuffer) {
    
}

void ShadowManager::updateSpotShadowMatrix(SpotLight* light, ShadowManager::ShadowData& shadowData) {
    shadowData.radius = light->shadowRadius();
    shadowData.bias = light->shadowBias();
    shadowData.intensity = light->shadowIntensity();
    
    auto viewMatrix = light->entity()->transform->worldMatrix().inverse();
    auto projMatrix = light->shadowProjectionMatrix();
    shadowData.vp[0] = projMatrix * viewMatrix;
    shadowData.cascadeSplits[0] = 1;
    shadowData.cascadeSplits[1] = -1; // mark cascade with negative sign
}

}
