//
//  skybox_material.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "skybox_material.h"

namespace vox {
bool SkyBoxMaterial::textureDecodeRGBM() {
    return _decodeParam.x;
}

void SkyBoxMaterial::setTextureDecodeRGBM(bool value) {
    _decodeParam.x = float(value);
}

float SkyBoxMaterial::RGBMDecodeFactor() {
    return _decodeParam.y;
}

void SkyBoxMaterial::setRGBMDecodeFactor(float value) {
    _decodeParam.y = value;
}

std::shared_ptr<MTL::Texture> SkyBoxMaterial::textureCubeMap() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(SkyBoxMaterial::_skyboxTextureProp));
}

void SkyBoxMaterial::setTextureCubeMap(std::shared_ptr<MTL::Texture> v) {
    shaderData.setData(SkyBoxMaterial::_skyboxTextureProp, v);
}

SkyBoxMaterial::SkyBoxMaterial() :
Material(Shader::find("skybox")),
_skyboxTextureProp(Shader::createProperty("u_skybox", ShaderDataGroup::Material)),
_mvpNoscaleProp(Shader::createProperty("u_mvpNoscale", ShaderDataGroup::Material)) {
    renderState.rasterState.cullMode = MTL::CullModeBack;
    renderState.depthState.compareFunction = MTL::CompareFunctionLessEqual;
}

}
