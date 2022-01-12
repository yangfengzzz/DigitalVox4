//
//  blinn_phong_material.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/29.
//

#include "blinn_phong_material.h"

namespace vox {
Imath::Color4f BlinnPhongMaterial::baseColor() {
    return std::any_cast<Imath::Color4f>(shaderData.getData(BlinnPhongMaterial::_diffuseColorProp));
}

void BlinnPhongMaterial::setBaseColor(const Imath::Color4f &newValue) {
    shaderData.setData(BlinnPhongMaterial::_diffuseColorProp, newValue);
}

std::shared_ptr<MTL::Texture> BlinnPhongMaterial::baseTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(BlinnPhongMaterial::_baseTextureProp));
}

void BlinnPhongMaterial::setBaseTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(BlinnPhongMaterial::_baseTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_DIFFUSE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_DIFFUSE_TEXTURE);
    }
}

Imath::Color4f BlinnPhongMaterial::specularColor() {
    return std::any_cast<Imath::Color4f>(shaderData.getData(BlinnPhongMaterial::_specularColorProp));
}

void BlinnPhongMaterial::setSpecularColor(const Imath::Color4f &newValue) {
    shaderData.setData(BlinnPhongMaterial::_specularColorProp, newValue);
}

std::shared_ptr<MTL::Texture> BlinnPhongMaterial::specularTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(BlinnPhongMaterial::_specularTextureProp));
}

void BlinnPhongMaterial::setSpecularTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(BlinnPhongMaterial::_specularTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_SPECULAR_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_SPECULAR_TEXTURE);
    }
}

Imath::Color4f BlinnPhongMaterial::emissiveColor() {
    return std::any_cast<Imath::Color4f>(shaderData.getData(BlinnPhongMaterial::_emissiveColorProp));
}

void BlinnPhongMaterial::setEmissiveColor(const Imath::Color4f &newValue) {
    shaderData.setData(BlinnPhongMaterial::_emissiveColorProp, newValue);
}

std::shared_ptr<MTL::Texture> BlinnPhongMaterial::emissiveTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(BlinnPhongMaterial::_emissiveTextureProp));
}

void BlinnPhongMaterial::BlinnPhongMaterial::setEmissiveTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(BlinnPhongMaterial::_emissiveTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_EMISSIVE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_EMISSIVE_TEXTURE);
    }
}

std::shared_ptr<MTL::Texture> BlinnPhongMaterial::normalTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(BlinnPhongMaterial::_normalTextureProp));
}

void BlinnPhongMaterial::setNormalTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(BlinnPhongMaterial::_normalTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_NORMAL_TEXTURE);
    }
}

float BlinnPhongMaterial::normalIntensity() {
    return std::any_cast<float>(shaderData.getData(BlinnPhongMaterial::_normalIntensityProp));
}

void BlinnPhongMaterial::setNormalIntensity(float newValue) {
    shaderData.setData(BlinnPhongMaterial::_normalIntensityProp, newValue);
}

float BlinnPhongMaterial::shininess() {
    return std::any_cast<float>(shaderData.getData(BlinnPhongMaterial::_shininessProp));
}

void BlinnPhongMaterial::setShininess(float newValue) {
    shaderData.setData(BlinnPhongMaterial::_shininessProp, newValue);
}

Imath::V4f BlinnPhongMaterial::tilingOffset() {
    return std::any_cast<Imath::V4f>(shaderData.getData(BlinnPhongMaterial::_tilingOffsetProp));
}

void BlinnPhongMaterial::setTilingOffset(const Imath::V4f &newValue) {
    shaderData.setData(BlinnPhongMaterial::_tilingOffsetProp, newValue);
}

BlinnPhongMaterial::BlinnPhongMaterial() :
BaseMaterial(Shader::find("blinn-phong")),
_diffuseColorProp(Shader::createProperty("u_diffuseColor", ShaderDataGroup::Material)),
_specularColorProp(Shader::createProperty("u_specularColor", ShaderDataGroup::Material)),
_emissiveColorProp(Shader::createProperty("u_emissiveColor", ShaderDataGroup::Material)),
_tilingOffsetProp(Shader::createProperty("u_tilingOffset", ShaderDataGroup::Material)),
_shininessProp(Shader::createProperty("u_shininess", ShaderDataGroup::Material)),
_normalIntensityProp(Shader::createProperty("u_normalIntensity", ShaderDataGroup::Material)),
_baseTextureProp(Shader::createProperty("u_diffuseTexture", ShaderDataGroup::Material)),
_specularTextureProp(Shader::createProperty("u_specularTexture", ShaderDataGroup::Material)),
_emissiveTextureProp(Shader::createProperty("u_emissiveTexture", ShaderDataGroup::Material)),
_normalTextureProp(Shader::createProperty("u_normalTexture", ShaderDataGroup::Material)){
    shaderData.enableMacro(NEED_WORLDPOS);
    shaderData.enableMacro(NEED_TILINGOFFSET);
    
    shaderData.setData(_diffuseColorProp, Imath::Color4f(1, 1, 1, 1));
    shaderData.setData(_specularColorProp, Imath::Color4f(1, 1, 1, 1));
    shaderData.setData(_emissiveColorProp, Imath::Color4f(0, 0, 0, 1));
    shaderData.setData(_tilingOffsetProp, Imath::V4f(1, 1, 0, 0));
    shaderData.setData(_shininessProp, 16.f);
    shaderData.setData(_normalIntensityProp, 1.f);
}

}
