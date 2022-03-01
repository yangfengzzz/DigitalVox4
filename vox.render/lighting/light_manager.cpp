//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light_manager.h"
#include "shader/shader.h"
#include "scene.h"
#include "metal_helpers.h"
#include <glog/logging.h>

namespace vox {
LightManager::LightManager() :
_pointLightProperty(Shader::createProperty("u_pointLight", ShaderDataGroup::Scene)),
_spotLightProperty(Shader::createProperty("u_spotLight", ShaderDataGroup::Scene)),
_directLightProperty(Shader::createProperty("u_directLight", ShaderDataGroup::Scene)) {
}

//MARK: - Point Light
void LightManager::attachPointLight(PointLight *light) {
    auto iter = std::find(_pointLights.begin(), _pointLights.end(), light);
    if (iter == _pointLights.end()) {
        _pointLights.push_back(light);
    } else {
        LOG(ERROR) << "Light already attached." << std::endl;;
    }
}

void LightManager::detachPointLight(PointLight *light) {
    auto iter = std::find(_pointLights.begin(), _pointLights.end(), light);
    if (iter != _pointLights.end()) {
        _pointLights.erase(iter);
    }
}

const std::vector<PointLight *> &LightManager::pointLights() const {
    return _pointLights;
}

//MARK: - Spot Light
void LightManager::attachSpotLight(SpotLight *light) {
    auto iter = std::find(_spotLights.begin(), _spotLights.end(), light);
    if (iter == _spotLights.end()) {
        _spotLights.push_back(light);
    } else {
        LOG(ERROR) << "Light already attached." << std::endl;;
    }
}

void LightManager::detachSpotLight(SpotLight *light) {
    auto iter = std::find(_spotLights.begin(), _spotLights.end(), light);
    if (iter != _spotLights.end()) {
        _spotLights.erase(iter);
    }
}

const std::vector<SpotLight *> &LightManager::spotLights() const {
    return _spotLights;
}

//MARK: - Direct Light
void LightManager::attachDirectLight(DirectLight *light) {
    auto iter = std::find(_directLights.begin(), _directLights.end(), light);
    if (iter == _directLights.end()) {
        _directLights.push_back(light);
    } else {
        LOG(ERROR) << "Light already attached." << std::endl;;
    }
}

void LightManager::detachDirectLight(DirectLight *light) {
    auto iter = std::find(_directLights.begin(), _directLights.end(), light);
    if (iter != _directLights.end()) {
        _directLights.erase(iter);
    }
}

const std::vector<DirectLight *> &LightManager::directLights() const {
    return _directLights;
}

//MARK: - Internal Uploader
void LightManager::updateShaderData(MTL::Device &device, ShaderData &shaderData) {
    size_t pointLightCount = _pointLights.size();
    _pointLightDatas.resize(pointLightCount);
    size_t spotLightCount = _spotLights.size();
    _spotLightDatas.resize(spotLightCount);
    size_t directLightCount = _directLights.size();
    _directLightDatas.resize(directLightCount);
    
    for (size_t i = 0; i < pointLightCount; i++) {
        _pointLights[i]->_updateShaderData(_pointLightDatas[i]);
    }
    
    for (size_t i = 0; i < spotLightCount; i++) {
        _spotLights[i]->_updateShaderData(_spotLightDatas[i]);
    }
    
    for (size_t i = 0; i < directLightCount; i++) {
        _directLights[i]->_updateShaderData(_directLightDatas[i]);
    }
    
    if (directLightCount) {
        if (_directLightBuffer == nullptr || _directLightBuffer->length() != sizeof(DirectLightData) * _directLightDatas.size()) {
            _directLightBuffer =
            CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(_directLightDatas.data(),
                                                                     _directLightDatas.size() * sizeof(DirectLightData),
                                                                     MTL::ResourceOptionCPUCacheModeDefault));
        } else {
            memcpy(_directLightBuffer->contents(), _directLightDatas.data(), _directLightDatas.size() * sizeof(DirectLightData));
        }
        
        shaderData.enableMacro(DIRECT_LIGHT_COUNT, std::make_pair(directLightCount, MTL::DataTypeInt));
        shaderData.setData(LightManager::_directLightProperty, _directLightBuffer);
    } else {
        shaderData.disableMacro(DIRECT_LIGHT_COUNT);
    }
    
    if (pointLightCount) {
        if (_pointLightBuffer == nullptr || _pointLightBuffer->length() != sizeof(PointLightData) * _pointLightDatas.size()) {
            _pointLightBuffer =
            CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(_pointLightDatas.data(),
                                                                     _pointLightDatas.size() * sizeof(PointLightData),
                                                                     MTL::ResourceOptionCPUCacheModeDefault));
        } else {
            memcpy(_pointLightBuffer->contents(), _pointLightDatas.data(), _pointLightDatas.size() * sizeof(PointLightData));
        }
        
        shaderData.enableMacro(POINT_LIGHT_COUNT, std::make_pair(pointLightCount, MTL::DataTypeInt));
        shaderData.setData(LightManager::_pointLightProperty, _pointLightBuffer);
    } else {
        shaderData.disableMacro(POINT_LIGHT_COUNT);
    }
    
    if (spotLightCount) {
        if (_spotLightBuffer == nullptr || _spotLightBuffer->length() != sizeof(SpotLightData) * _spotLightDatas.size()) {
            _spotLightBuffer =
            CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(_spotLightDatas.data(),
                                                                     _spotLightDatas.size() * sizeof(SpotLightData),
                                                                     MTL::ResourceOptionCPUCacheModeDefault));
        } else {
            memcpy(_spotLightBuffer->contents(), _spotLightDatas.data(), _spotLightDatas.size() * sizeof(SpotLightData));
        }
        
        shaderData.enableMacro(SPOT_LIGHT_COUNT, std::make_pair(spotLightCount, MTL::DataTypeInt));
        shaderData.setData(LightManager::_spotLightProperty, _spotLightBuffer);
    } else {
        shaderData.disableMacro(SPOT_LIGHT_COUNT);
    }
}


}
