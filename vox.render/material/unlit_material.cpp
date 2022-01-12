//
//  unlit_material.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/29.
//

#include "unlit_material.h"

namespace vox {
Imath::Color4f UnlitMaterial::baseColor() {
    return std::any_cast<Imath::Color4f>(shaderData.getData(_baseColorProp));
}

void UnlitMaterial::setBaseColor(const Imath::Color4f &newValue) {
    shaderData.setData(UnlitMaterial::_baseColorProp, newValue);
}

std::shared_ptr<MTL::Texture> UnlitMaterial::baseTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(_baseTextureProp));
}

void UnlitMaterial::setBaseTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(UnlitMaterial::_baseTextureProp, newValue);
    
    if (newValue) {
        shaderData.enableMacro(HAS_BASE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_BASE_TEXTURE);
    }
}

Imath::V4f UnlitMaterial::tilingOffset() {
    return std::any_cast<Imath::V4f>(shaderData.getData(_tilingOffsetProp));
}

void UnlitMaterial::setTilingOffset(const Imath::V4f &newValue) {
    shaderData.setData(UnlitMaterial::_tilingOffsetProp, newValue);
}

UnlitMaterial::UnlitMaterial() :
BaseMaterial(Shader::find("unlit")),
_baseColorProp(Shader::createProperty("u_baseColor", ShaderDataGroup::Material)),
_baseTextureProp(Shader::createProperty("u_baseTexture", ShaderDataGroup::Material)),
_tilingOffsetProp(Shader::createProperty("u_tilingOffset", ShaderDataGroup::Material)) {
    shaderData.enableMacro(OMIT_NORMAL);
    shaderData.enableMacro(NEED_TILINGOFFSET);
    
    shaderData.setData(_baseColorProp, Imath::Color4f(1, 1, 1, 1));
    shaderData.setData(_tilingOffsetProp, Imath::V4f(1, 1, 0, 0));
}

}
