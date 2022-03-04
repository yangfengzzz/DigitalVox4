//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_material.h"

namespace vox {
ParticleMaterial::ParticleMaterial():
BaseMaterial(Shader::find("particle")),
_minParticleSizeProp(Shader::createProperty("uMinParticleSize", ShaderDataGroup::Material)),
_maxParticleSizeProp(Shader::createProperty("uMaxParticleSize", ShaderDataGroup::Material)),
_colorModeProp(Shader::createProperty("uColorMode", ShaderDataGroup::Material)),
_birthGradientProp(Shader::createProperty("uBirthGradient", ShaderDataGroup::Material)),
_deathGradientProp(Shader::createProperty("uDeathGradient", ShaderDataGroup::Material)),
_fadeCoefficientProp(Shader::createProperty("uFadeCoefficient", ShaderDataGroup::Material)),
_debugDrawProp(Shader::createProperty("uDebugDraw", ShaderDataGroup::Material)) {
    setColorMode(ColorMode::DEFAULT);    
    setIsTransparent(true);
    setBlendMode(BlendMode::Additive);
}

float ParticleMaterial::minParticleSize() const {
    return std::any_cast<float>(shaderData.getData(_minParticleSizeProp));
}

void ParticleMaterial::setMinParticleSize(float size) {
    shaderData.setData(_minParticleSizeProp, size);
}

float ParticleMaterial::maxParticleSize() const {
    return std::any_cast<float>(shaderData.getData(_maxParticleSizeProp));
}

void ParticleMaterial::setMaxParticleSize(float size) {
    shaderData.setData(_maxParticleSizeProp, size);
}

ParticleMaterial::ColorMode ParticleMaterial::colorMode() const {
    return (ColorMode)std::any_cast<uint32_t>(shaderData.getData(_colorModeProp));
}

void ParticleMaterial::setColorMode(ColorMode mode) {
    shaderData.setData(_colorModeProp, (uint32_t)mode);
}

Vector3F ParticleMaterial::birthGradient() const {
    return std::any_cast<Vector3F>(shaderData.getData(_birthGradientProp));
}

void ParticleMaterial::setBirthGradient(const Vector3F& gradient) {
    shaderData.setData(_birthGradientProp, gradient);
}

Vector3F ParticleMaterial::deathGradient() const {
    return std::any_cast<Vector3F>(shaderData.getData(_deathGradientProp));
}

void ParticleMaterial::setDeathGradient(const Vector3F& gradient) {
    shaderData.setData(_deathGradientProp, gradient);
}

float ParticleMaterial::fadeCoefficient() const {
    return std::any_cast<float>(shaderData.getData(_fadeCoefficientProp));
}

void ParticleMaterial::setFadeCoefficient(float coeff) {
    shaderData.setData(_fadeCoefficientProp, coeff);
}

bool ParticleMaterial::debugDraw() const {
    return std::any_cast<bool>(shaderData.getData(_debugDrawProp));
}

void ParticleMaterial::setDebugDraw(bool flag) {
    shaderData.setData(_debugDrawProp, flag);
}

}
