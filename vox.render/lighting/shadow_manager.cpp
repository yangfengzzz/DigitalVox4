//
//  shadow_manager.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/22.
//

#include "shadow_manager.h"
#include "shader/shader.h"

namespace vox {
ShadowManager::ShadowManager(MTL::Library& library, Scene* scene):
_library(library),
_scene(scene),
_resourceLoader(*scene->device()),
_shadowMapProp(Shader::createProperty("u_shadowMap", ShaderDataGroup::Scene)),
_cubeShadowMapProp(Shader::createProperty("u_cubeShadowMap", ShaderDataGroup::Scene)),
_shadowDataProp(Shader::createProperty("u_shadowData", ShaderDataGroup::Scene)),
_cubeShadowDataProp(Shader::createProperty("u_cubeShadowData", ShaderDataGroup::Scene)) {
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

float ShadowManager::cascadeSplitLambda() {
    return _cascadeSplitLambda;
}

void ShadowManager::setCascadeSplitLambda(float value) {
    _cascadeSplitLambda = value;
}

void ShadowManager::draw(MTL::CommandBuffer& commandBuffer) {
    _shadowCount = 0;
    _drawSpotShadowMap(commandBuffer);
    _drawDirectShadowMap(commandBuffer);
    if (_shadowCount) {
        _packedTexture = commandBuffer.createTextureArray(_shadowMaps.begin(), _shadowMaps.begin() + _shadowCount, _packedTexture);
        _scene->shaderData.setData(_shadowMapProp, _packedTexture);
        _scene->shaderData.setData(_shadowDataProp, _shadowDatas);
    }
    _cubeShadowCount = 0;
    _drawPointShadowMap(commandBuffer);
    if (_cubeShadowCount) {
        _packedCubeTexture = commandBuffer.createCubeTextureArray(_cubeShadowMaps.begin(), _cubeShadowMaps.begin() + _cubeShadowCount, _packedCubeTexture);
        _scene->shaderData.setData(_cubeShadowMapProp, _packedCubeTexture);
        _scene->shaderData.setData(_cubeShadowDataProp, _cubeShadowDatas);
    }
}

void ShadowManager::_drawSpotShadowMap(MTL::CommandBuffer& commandBuffer) {
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

            _updateSpotShadowMatrix(light, _shadowDatas[_shadowCount]);
            _shadowSubpass->setViewProjectionMatrix(_shadowDatas[_shadowCount].vp[0]);
            _renderPass->draw(commandBuffer, "Spot Shadow Pass");
            _shadowCount++;
        }
    }
}

void ShadowManager::_drawDirectShadowMap(MTL::CommandBuffer& commandBuffer) {
    
}

void ShadowManager::_drawPointShadowMap(MTL::CommandBuffer& commandBuffer) {
    
}

void ShadowManager::_updateSpotShadowMatrix(SpotLight* light, ShadowManager::ShadowData& shadowData) {
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
