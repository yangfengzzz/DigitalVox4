//
//  pbr_specular_material.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/29.
//

#include "pbr_specular_material.h"

namespace vox {
Color PBRSpecularMaterial::specularColor() {
    return std::any_cast<Color>(shaderData.getData(PBRSpecularMaterial::_specularColorProp));
}

void PBRSpecularMaterial::setSpecularColor(const Color &newValue) {
    shaderData.setData(PBRSpecularMaterial::_specularColorProp, newValue);
}

float PBRSpecularMaterial::glossiness() {
    return std::any_cast<float>(shaderData.getData(PBRSpecularMaterial::_glossinessProp));
}

void PBRSpecularMaterial::setGlossiness(float newValue) {
    shaderData.setData(PBRSpecularMaterial::_glossinessProp, newValue);
}

std::shared_ptr<MTL::Texture> PBRSpecularMaterial::specularGlossinessTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(PBRSpecularMaterial::_specularGlossinessTextureProp));
}

void PBRSpecularMaterial::setSpecularGlossinessTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(PBRSpecularMaterial::_specularGlossinessTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_SPECULARGLOSSINESSMAP);
    } else {
        shaderData.disableMacro(HAS_SPECULARGLOSSINESSMAP);
    }
}

PBRSpecularMaterial::PBRSpecularMaterial() :
PBRBaseMaterial(),
_glossinessProp(Shader::createProperty("u_glossiness", ShaderDataGroup::Material)),
_specularColorProp(Shader::createProperty("u_specularColor", ShaderDataGroup::Material)),
_specularGlossinessTextureProp(Shader::createProperty("_specularGlossinessTexture", ShaderDataGroup::Material)) {
    shaderData.setData(PBRSpecularMaterial::_specularColorProp, Color(1, 1, 1, 1));
    shaderData.setData(PBRSpecularMaterial::_glossinessProp, 1.f);
}

}
