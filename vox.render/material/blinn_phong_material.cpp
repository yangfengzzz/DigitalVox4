//
//  blinn_phong_material.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/29.
//

#include "blinn_phong_material.h"

namespace vox {
Color BlinnPhongMaterial::baseColor() {
    return std::any_cast<Color>(shaderData.getData(BlinnPhongMaterial::_diffuseColorProp));
}

void BlinnPhongMaterial::setBaseColor(const Color &newValue) {
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

Color BlinnPhongMaterial::specularColor() {
    return std::any_cast<Color>(shaderData.getData(BlinnPhongMaterial::_specularColorProp));
}

void BlinnPhongMaterial::setSpecularColor(const Color &newValue) {
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

Color BlinnPhongMaterial::emissiveColor() {
    return std::any_cast<Color>(shaderData.getData(BlinnPhongMaterial::_emissiveColorProp));
}

void BlinnPhongMaterial::setEmissiveColor(const Color &newValue) {
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

Vector4F BlinnPhongMaterial::tilingOffset() {
    return std::any_cast<Vector4F>(shaderData.getData(BlinnPhongMaterial::_tilingOffsetProp));
}

void BlinnPhongMaterial::setTilingOffset(const Vector4F &newValue) {
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
    
    shaderData.setData(_diffuseColorProp, Color(1, 1, 1, 1));
    shaderData.setData(_specularColorProp, Color(1, 1, 1, 1));
    shaderData.setData(_emissiveColorProp, Color(0, 0, 0, 1));
    shaderData.setData(_tilingOffsetProp, Vector4F(1, 1, 0, 0));
    shaderData.setData(_shininessProp, 16.f);
    shaderData.setData(_normalIntensityProp, 1.f);
}

}
