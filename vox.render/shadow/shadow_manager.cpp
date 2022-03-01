//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_manager.h"
#include "shader/shader.h"
#include "camera.h"
#include "matrix_utils.h"
#include "metal_helpers.h"
#include "texture/texture_utils.h"
#include "texture/sampled_texture2d.h"
#include "texture/sampled_texturecube.h"

namespace vox {
uint32_t ShadowManager::_shadowCount = 0;
uint32_t ShadowManager::_cubeShadowCount = 0;

uint32_t ShadowManager::shadowCount() {
    return _shadowCount;
}

uint32_t ShadowManager::cubeShadowCount() {
    return _cubeShadowCount;
}

ShadowManager::ShadowManager(MTL::Library &library, Scene *scene, Camera *camera) :
_library(library),
_scene(scene),
_camera(camera),
_shadowMapProp(Shader::createProperty("u_shadowMap", ShaderDataGroup::Scene)),
_cubeShadowMapProp(Shader::createProperty("u_cubeShadowMap", ShaderDataGroup::Scene)),
_shadowDataProp(Shader::createProperty("u_shadowData", ShaderDataGroup::Scene)),
_cubeShadowDataProp(Shader::createProperty("u_cubeShadowData", ShaderDataGroup::Scene)) {
    scene->registerVertexUploader<std::array<ShadowManager::ShadowData, MAX_SHADOW>>([](const std::array<ShadowManager::ShadowData, MAX_SHADOW> &x,
                                                                                        size_t location, MTL::RenderCommandEncoder &encoder) {
        encoder.setVertexBytes(&x, sizeof(std::array<ShadowManager::ShadowData, MAX_SHADOW>), location);
    });
    
    scene->registerFragmentUploader<std::array<ShadowManager::ShadowData, MAX_SHADOW>>([](const std::array<ShadowManager::ShadowData, MAX_SHADOW> &x,
                                                                                          size_t location, MTL::RenderCommandEncoder &encoder) {
        encoder.setFragmentBytes(&x, sizeof(std::array<ShadowManager::ShadowData, MAX_SHADOW>), location);
    });
    
    scene->registerVertexUploader<std::array<ShadowManager::CubeShadowData, MAX_CUBE_SHADOW>>([](const std::array<ShadowManager::CubeShadowData, MAX_CUBE_SHADOW> &x,
                                                                                                 size_t location, MTL::RenderCommandEncoder &encoder) {
        encoder.setVertexBytes(&x, sizeof(std::array<CubeShadowData, MAX_CUBE_SHADOW>), location);
    });
    
    scene->registerFragmentUploader<std::array<ShadowManager::CubeShadowData, MAX_CUBE_SHADOW>>([](const std::array<ShadowManager::CubeShadowData, MAX_CUBE_SHADOW> &x,
                                                                                                   size_t location, MTL::RenderCommandEncoder &encoder) {
        encoder.setFragmentBytes(&x, sizeof(std::array<CubeShadowData, MAX_CUBE_SHADOW>), location);
    });
    
    _renderPassDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::RenderPassDescriptor, MTL::RenderPassDescriptor::alloc()->init());
    _renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
    _renderPass = std::make_unique<RenderPass>(_library, *_renderPassDescriptor);
    auto shadowSubpass = std::make_unique<ShadowSubpass>(nullptr, _scene, _camera);
    _shadowSubpass = shadowSubpass.get();
    _renderPass->addSubpass(std::move(shadowSubpass));
}

float ShadowManager::cascadeSplitLambda() {
    return _cascadeSplitLambda;
}

void ShadowManager::setCascadeSplitLambda(float value) {
    _cascadeSplitLambda = value;
}

void ShadowManager::draw(MTL::CommandBuffer &commandBuffer) {
    _shadowCount = 0;
    _drawSpotShadowMap(commandBuffer);
    _drawDirectShadowMap(commandBuffer);
    if (_shadowCount) {
        if (!_packedTexture || _packedTexture->arrayLength() != _shadowCount) {
            _packedTexture = std::make_shared<SampledTexture2D>(_scene->device(), SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION,
                                                                _shadowCount, false, SHADOW_MAP_FORMAT,
                                                                MTL::TextureUsageShaderRead);
            _packedTexture->setTextureViewDimension(MTL::TextureType2DArray);
            _packedTexture->setCompareFunction(MTL::CompareFunctionLess);
            _packedTexture->setAddressModeU(MTL::SamplerAddressModeClampToEdge);
            _packedTexture->setAddressModeV(MTL::SamplerAddressModeClampToEdge);
        }
        TextureUtils::buildTextureArray(_shadowMaps.begin(), _shadowMaps.begin() + _shadowCount,
                                        SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION,
                                        _packedTexture->texture(), commandBuffer);
        _scene->shaderData.setSampledTexure(_shadowMapProp, _packedTexture);
        _scene->shaderData.setData(_shadowDataProp, _shadowDatas);
    }
    
    _cubeShadowCount = 0;
    _drawPointShadowMap(commandBuffer);
    if (_cubeShadowCount) {
        if (!_packedCubeTexture || _packedCubeTexture->arrayLength() != _cubeShadowCount) {
            _packedCubeTexture = std::make_shared<SampledTextureCube>(_scene->device(), SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION,
                                                                      _cubeShadowCount, false, SHADOW_MAP_FORMAT,
                                                                      MTL::TextureUsageShaderRead);
            _packedCubeTexture->setTextureViewDimension(MTL::TextureTypeCubeArray);
            _packedCubeTexture->setCompareFunction(MTL::CompareFunctionLess);
            _packedCubeTexture->setAddressModeU(MTL::SamplerAddressModeClampToEdge);
            _packedCubeTexture->setAddressModeV(MTL::SamplerAddressModeClampToEdge);
        }
        TextureUtils::buildCubeTextureArray(_cubeShadowMaps.begin(), _cubeShadowMaps.begin() + _cubeShadowCount,
                                            SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION,
                                            _packedCubeTexture->texture(), commandBuffer);
        
        _scene->shaderData.setSampledTexure(_cubeShadowMapProp, _packedCubeTexture);
        _scene->shaderData.setData(_cubeShadowDataProp, _cubeShadowDatas);
    }
}

void ShadowManager::_drawSpotShadowMap(MTL::CommandBuffer &commandBuffer) {
    const auto &lights = _scene->light_manager.spotLights();
    for (const auto &light: lights) {
        if (light->enableShadow() && _shadowCount < MAX_SHADOW) {
            _updateSpotShadow(light, _shadowDatas[_shadowCount]);
            
            std::shared_ptr<MTL::Texture> texture{nullptr};
            if (_shadowCount < _shadowMaps.size()) {
                texture = _shadowMaps[_shadowCount];
            } else {
                auto descriptor = CLONE_METAL_CUSTOM_DELETER(MTL::TextureDescriptor, MTL::TextureDescriptor::alloc()->init());
                descriptor->setTextureType(MTL::TextureType::TextureType2D);
                descriptor->setPixelFormat(SHADOW_MAP_FORMAT);
                descriptor->setWidth(SHADOW_MAP_RESOLUTION);
                descriptor->setHeight(SHADOW_MAP_RESOLUTION);
                descriptor->setStorageMode(MTL::StorageMode::StorageModePrivate);
                descriptor->setUsage(MTL::TextureUsageRenderTarget);
                texture = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, _scene->device().newTexture(descriptor.get()));
                _shadowMaps.push_back(texture);
            }
            
            {
                _renderPassDescriptor->depthAttachment()->setTexture(texture.get());
                _shadowSubpass->setViewProjectionMatrix(_shadowDatas[_shadowCount].vp[0]);
                _renderPass->draw(commandBuffer, "Spot Shadow Pass");
            }
            _shadowCount++;
        }
    }
}

void ShadowManager::_drawDirectShadowMap(MTL::CommandBuffer &commandBuffer) {
    const auto &lights = _scene->light_manager.directLights();
    for (const auto &light: lights) {
        if (light->enableShadow() && _shadowCount < MAX_SHADOW) {
            _updateCascadesShadow(light, _shadowDatas[_shadowCount]);
            
            std::shared_ptr<MTL::Texture> texture{nullptr};
            if (_shadowCount < _shadowMaps.size()) {
                texture = _shadowMaps[_shadowCount];
            } else {
                auto descriptor = CLONE_METAL_CUSTOM_DELETER(MTL::TextureDescriptor, MTL::TextureDescriptor::alloc()->init());
                descriptor->setTextureType(MTL::TextureType::TextureType2D);
                descriptor->setPixelFormat(SHADOW_MAP_FORMAT);
                descriptor->setWidth(SHADOW_MAP_RESOLUTION);
                descriptor->setHeight(SHADOW_MAP_RESOLUTION);
                descriptor->setStorageMode(MTL::StorageMode::StorageModePrivate);
                descriptor->setUsage(MTL::TextureUsageRenderTarget);
                texture = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, _scene->device().newTexture(descriptor.get()));
                _shadowMaps.push_back(texture);
            }
            _renderPassDescriptor->depthAttachment()->setTexture(texture.get());
            
            for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
                _shadowSubpass->setViewport(_viewport[i]);
                if (i == 0) {
                    _renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadAction::LoadActionClear);
                } else {
                    _renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadAction::LoadActionLoad);
                }
                _shadowSubpass->setViewProjectionMatrix(_shadowDatas[_shadowCount].vp[i]);
                _renderPass->draw(commandBuffer, "Direct Shadow Pass");
            }
            _shadowCount++;
        }
    }
    _renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadAction::LoadActionClear);
    _shadowSubpass->setViewport(std::nullopt);
}

void ShadowManager::_drawPointShadowMap(MTL::CommandBuffer &commandBuffer) {
    const auto &lights = _scene->light_manager.pointLights();
    for (const auto &light: lights) {
        if (light->enableShadow() && _cubeShadowCount < MAX_CUBE_SHADOW) {
            _updatePointShadow(light, _cubeShadowDatas[_cubeShadowCount]);
            
            std::shared_ptr<MTL::Texture> texture{nullptr};
            if (_cubeShadowCount < _cubeShadowMaps.size()) {
                texture = _cubeShadowMaps[_cubeShadowCount];
            } else {
                auto descriptor = CLONE_METAL_CUSTOM_DELETER(MTL::TextureDescriptor, MTL::TextureDescriptor::alloc()->init());
                descriptor->setTextureType(MTL::TextureType::TextureTypeCube);
                descriptor->setPixelFormat(SHADOW_MAP_FORMAT);
                descriptor->setWidth(SHADOW_MAP_RESOLUTION);
                descriptor->setHeight(SHADOW_MAP_RESOLUTION);
                descriptor->setStorageMode(MTL::StorageMode::StorageModePrivate);
                descriptor->setUsage(MTL::TextureUsageRenderTarget);
                texture = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, _scene->device().newTexture(descriptor.get()));
                _cubeShadowMaps.push_back(texture);
            }
            
            for (int i = 0; i < 6; i++) {
                auto textureView = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, texture->newTextureView(SHADOW_MAP_FORMAT, MTL::TextureType2D,
                                                                                                    NS::Range(0, 1), NS::Range(i, 1)));
                _renderPassDescriptor->depthAttachment()->setTexture(textureView.get());
                _shadowSubpass->setViewProjectionMatrix(_cubeShadowDatas[_cubeShadowCount].vp[i]);
                _renderPass->draw(commandBuffer, "Point Shadow Pass");
            }
            _cubeShadowCount++;
        }
    }
}

void ShadowManager::_updateSpotShadow(SpotLight *light, ShadowManager::ShadowData &shadowData) {
    shadowData.radius = light->shadowRadius();
    shadowData.bias = light->shadowBias();
    shadowData.intensity = light->shadowIntensity();
    
    auto viewMatrix = light->entity()->transform->worldMatrix().inverse();
    auto projMatrix = light->shadowProjectionMatrix();
    shadowData.vp[0] = projMatrix * viewMatrix;
    shadowData.cascadeSplits[0] = 1;
    shadowData.cascadeSplits[1] = -1; // mark cascade with negative sign
}

void ShadowManager::_updateCascadesShadow(DirectLight *light, ShadowManager::ShadowData &shadowData) {
    shadowData.radius = light->shadowRadius();
    shadowData.bias = light->shadowBias();
    shadowData.intensity = light->shadowIntensity();
    
    std::array<float, SHADOW_MAP_CASCADE_COUNT> cascadeSplits{};
    auto worldPos = light->entity()->transform->worldPosition();
    
    float nearClip = _camera->nearClipPlane();
    float farClip = _camera->farClipPlane();
    float clipRange = farClip - nearClip;
    
    float minZ = nearClip;
    float maxZ = nearClip + clipRange;
    
    float range = maxZ - minZ;
    float ratio = maxZ / minZ;
    
    // Calculate split depths based on view camera frustum
    // Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
    for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
        float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
        float log = minZ * std::pow(ratio, p);
        float uniform = minZ + range * p;
        float d = _cascadeSplitLambda * (log - uniform) + uniform;
        cascadeSplits[i] = (d - nearClip) / clipRange;
    }
    
    std::array<Point3F, 8> frustumCorners = {
        Point3F(-1.0f, 1.0f, 0.0f),
        Point3F(1.0f, 1.0f, 0.0f),
        Point3F(1.0f, -1.0f, 0.0f),
        Point3F(-1.0f, -1.0f, 0.0f),
        Point3F(-1.0f, 1.0f, 1.0f),
        Point3F(1.0f, 1.0f, 1.0f),
        Point3F(1.0f, -1.0f, 1.0f),
        Point3F(-1.0f, -1.0f, 1.0f),
    };
    
    // Project frustum corners into world space
    Matrix4x4F invCam = (_camera->projectionMatrix() * _camera->viewMatrix()).inverse();
    for (uint32_t i = 0; i < 8; i++) {
        frustumCorners[i] = invCam * frustumCorners[i];
    }
    
    // Calculate orthographic projection matrix for each cascade
    float lastSplitDist = 0.0;
    for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
        float splitDist = cascadeSplits[i];
        std::array<Point3F, 8> _frustumCorners = frustumCorners;
        
        for (uint32_t i = 0; i < 4; i++) {
            Vector3F dist = _frustumCorners[i + 4] - _frustumCorners[i];
            _frustumCorners[i + 4] = _frustumCorners[i] + (dist * splitDist);
            _frustumCorners[i] = _frustumCorners[i] + (dist * lastSplitDist);
        }
        
        auto lightMat = light->entity()->transform->worldMatrix();
        auto lightViewMat = lightMat.inverse();
        for (uint32_t i = 0; i < 8; i++) {
            _frustumCorners[i] = lightViewMat * _frustumCorners[i];
        }
        float farDist = _frustumCorners[7].distanceTo(_frustumCorners[5]);
        float crossDist = _frustumCorners[7].distanceTo(_frustumCorners[1]);
        float maxDist = farDist > crossDist ? farDist : crossDist;
        
        float minX = std::numeric_limits<float>::infinity();
        float maxX = -std::numeric_limits<float>::infinity();
        float minY = std::numeric_limits<float>::infinity();
        float maxY = -std::numeric_limits<float>::infinity();
        float minZ = std::numeric_limits<float>::infinity();
        float maxZ = -std::numeric_limits<float>::infinity();
        for (uint32_t i = 0; i < 8; i++) {
            minX = std::min(minX, _frustumCorners[i].x);
            maxX = std::max(maxX, _frustumCorners[i].x);
            minY = std::min(minY, _frustumCorners[i].y);
            maxY = std::max(maxY, _frustumCorners[i].y);
            minZ = std::min(minZ, _frustumCorners[i].z);
            maxZ = std::max(maxZ, _frustumCorners[i].z);
        }
        
        // texel tile
        float fWorldUnitsPerTexel = maxDist / (float) 1000;
        float posX = (minX + maxX) * 0.5f;
        posX /= fWorldUnitsPerTexel;
        posX = std::floor(posX);
        posX *= fWorldUnitsPerTexel;
        
        float posY = (minY + maxY) * 0.5f;
        posY /= fWorldUnitsPerTexel;
        posY = std::floor(posY);
        posY *= fWorldUnitsPerTexel;
        
        float posZ = maxZ;
        posZ /= fWorldUnitsPerTexel;
        posZ = std::floor(posZ);
        posZ *= fWorldUnitsPerTexel;
        
        Point3F center = Point3F(posX, posY, posZ);
        center = lightMat * center;
        light->entity()->transform->setWorldPosition(center);
        
        float radius = maxDist / 2.0;
        Vector3F maxExtents = Vector3F(radius, radius, radius);
        Vector3F minExtents = -maxExtents;
        Matrix4x4F lightOrthoMatrix = makeOrtho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxZ - minZ);
        
        // Store split distance and matrix in cascade
        shadowData.cascadeSplits[i] = (_camera->nearClipPlane() + splitDist * clipRange) * -1.0f;
        shadowData.vp[i] = lightOrthoMatrix * light->entity()->transform->worldMatrix().inverse();
        light->entity()->transform->setWorldPosition(worldPos);
        lastSplitDist = cascadeSplits[i];
    }
}

void ShadowManager::_updatePointShadow(PointLight *light, ShadowManager::CubeShadowData &shadowData) {
    shadowData.radius = light->shadowRadius();
    shadowData.bias = light->shadowBias();
    shadowData.intensity = light->shadowIntensity();
    
    auto projMatrix = light->shadowProjectionMatrix();
    auto worldPos = light->entity()->transform->worldPosition();
    shadowData.lightPos = Vector4F(worldPos.x, worldPos.y, worldPos.z, 1.0);
    
    for (int i = 0; i < 6; i++) {
        light->entity()->transform->lookAt(worldPos + _cubeMapDirection[i].first, _cubeMapDirection[i].second);
        auto viewMatrix = light->entity()->transform->worldMatrix().inverse();
        shadowData.vp[i] = projMatrix * viewMatrix;
    }
}

}
