//
//  pbr_base_material.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/29.
//

#include "pbr_base_material.h"

namespace vox {
Imath::Color4f PBRBaseMaterial::baseColor() {
    return std::any_cast<Imath::Color4f>(shaderData.getData(PBRBaseMaterial::_baseColorProp));
}

void PBRBaseMaterial::setBaseColor(const Imath::Color4f &newValue) {
    shaderData.setData(PBRBaseMaterial::_baseColorProp, newValue);
}

std::shared_ptr<MTL::Texture> PBRBaseMaterial::baseTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(PBRBaseMaterial::_baseTextureProp));
}

void PBRBaseMaterial::setBaseTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(PBRBaseMaterial::_baseTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_BASE_COLORMAP);
    } else {
        shaderData.disableMacro(HAS_BASE_COLORMAP);
    }
}

std::shared_ptr<MTL::Texture> PBRBaseMaterial::normalTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(PBRBaseMaterial::_normalTextureProp));
}

void PBRBaseMaterial::setNormalTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(PBRBaseMaterial::_normalTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_NORMAL_TEXTURE);
    }
}

float PBRBaseMaterial::normalTextureIntensity() {
    return std::any_cast<float>(shaderData.getData(PBRBaseMaterial::_normalTextureIntensityProp));
}

void PBRBaseMaterial::setNormalTextureIntensity(float newValue) {
    shaderData.setData(PBRBaseMaterial::_normalTextureIntensityProp, newValue);
}

Imath::Color4f PBRBaseMaterial::emissiveColor() {
    return std::any_cast<Imath::Color4f>(shaderData.getData(PBRBaseMaterial::_emissiveColorProp));
}

void PBRBaseMaterial::setEmissiveColor(const Imath::Color4f &newValue) {
    shaderData.setData(PBRBaseMaterial::_emissiveColorProp, newValue);
}

std::shared_ptr<MTL::Texture> PBRBaseMaterial::emissiveTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(PBRBaseMaterial::_emissiveTextureProp));
}

void PBRBaseMaterial::setEmissiveTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(PBRBaseMaterial::_emissiveTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_EMISSIVEMAP);
    } else {
        shaderData.disableMacro(HAS_EMISSIVEMAP);
    }
}

std::shared_ptr<MTL::Texture> PBRBaseMaterial::occlusionTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(PBRBaseMaterial::_occlusionTextureProp));
}

void PBRBaseMaterial::setOcclusionTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(PBRBaseMaterial::_occlusionTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_OCCLUSIONMAP);
    } else {
        shaderData.disableMacro(HAS_OCCLUSIONMAP);
    }
}

float PBRBaseMaterial::occlusionTextureIntensity() {
    return std::any_cast<float>(shaderData.getData(PBRBaseMaterial::_occlusionTextureIntensityProp));
}

void PBRBaseMaterial::setOcclusionTextureIntensity(float newValue) {
    shaderData.setData(PBRBaseMaterial::_occlusionTextureIntensityProp, newValue);
}

Imath::V4f PBRBaseMaterial::tilingOffset() {
    return std::any_cast<Imath::V4f>(shaderData.getData(PBRBaseMaterial::_tilingOffsetProp));
}

void PBRBaseMaterial::setTilingOffset(const Imath::V4f &newValue) {
    shaderData.setData(PBRBaseMaterial::_tilingOffsetProp, newValue);
}

PBRBaseMaterial::PBRBaseMaterial() :
BaseMaterial(Shader::find("pbr")),
_tilingOffsetProp(Shader::createProperty("u_tilingOffset", ShaderDataGroup::Material)),
_normalTextureIntensityProp(Shader::createProperty("u_normalIntensity", ShaderDataGroup::Material)),
_occlusionTextureIntensityProp(Shader::createProperty("u_occlusionStrength", ShaderDataGroup::Material)),
_baseColorProp(Shader::createProperty("u_baseColor", ShaderDataGroup::Material)),
_emissiveColorProp(Shader::createProperty("u_emissiveColor", ShaderDataGroup::Material)),
_baseTextureProp(Shader::createProperty("u_baseColorTexture", ShaderDataGroup::Material)),
_normalTextureProp(Shader::createProperty("u_normalTexture", ShaderDataGroup::Material)),
_emissiveTextureProp(Shader::createProperty("u_emissiveTexture", ShaderDataGroup::Material)),
_occlusionTextureProp(Shader::createProperty("u_occlusionTexture", ShaderDataGroup::Material)) {
    shaderData.enableMacro(NEED_WORLDPOS);
    shaderData.enableMacro(NEED_TILINGOFFSET);
    
    shaderData.setData(PBRBaseMaterial::_baseColorProp, Imath::Color4f(1, 1, 1, 1));
    shaderData.setData(PBRBaseMaterial::_emissiveColorProp, Imath::Color4f(0, 0, 0, 1));
    shaderData.setData(PBRBaseMaterial::_tilingOffsetProp, Imath::V4f(1, 1, 0, 0));
    
    shaderData.setData(PBRBaseMaterial::_normalTextureIntensityProp, 1.f);
    shaderData.setData(PBRBaseMaterial::_occlusionTextureIntensityProp, 1.f);
}

}
