//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle.h"
#include "entity.h"
#include "scene.h"
#include "mesh/mesh_renderer.h"
#include "particle_manager.h"

namespace vox {
Particle::Particle(Entity *entity) :
Component(entity),
_timeStepProp(Shader::createProperty("uTimeStep", ShaderDataGroup::Compute)),
_vectorFieldTextureProp(Shader::createProperty("uVectorFieldTexture", ShaderDataGroup::Compute)),
_boundingVolumeProp(Shader::createProperty("uBoundingVolume", ShaderDataGroup::Compute)),
_bboxSizeProp(Shader::createProperty("uBBoxSize", ShaderDataGroup::Compute)),
_scatteringFactorProp(Shader::createProperty("uScatteringFactor", ShaderDataGroup::Compute)),
_vectorFieldFactorProp(Shader::createProperty("uVectorFieldFactor", ShaderDataGroup::Compute)),
_curlNoiseFactorProp(Shader::createProperty("uCurlNoiseFactor", ShaderDataGroup::Compute)),
_curlNoiseScaleProp(Shader::createProperty("uCurlNoiseScale", ShaderDataGroup::Compute)),
_velocityFactorProp(Shader::createProperty("uVelocityFactor", ShaderDataGroup::Compute)),

_emitCountProp(Shader::createProperty("uEmitCount", ShaderDataGroup::Compute)),
_emitterTypeProp(Shader::createProperty("uEmitterType", ShaderDataGroup::Compute)),
_emitterPositionProp(Shader::createProperty("uEmitterPosition", ShaderDataGroup::Compute)),
_emitterDirectionProp(Shader::createProperty("uEmitterDirection", ShaderDataGroup::Compute)),
_emitterRadiusProp(Shader::createProperty("uEmitterRadius", ShaderDataGroup::Compute)),
_particleMinAgeProp(Shader::createProperty("uParticleMinAge", ShaderDataGroup::Compute)),
_particleMaxAgeProp(Shader::createProperty("uParticleMaxAge", ShaderDataGroup::Compute)) {
    auto renderer = entity->addComponent<MeshRenderer>();
    _material = std::make_shared<ParticleMaterial>();
    renderer->setMaterial(_material);
}

ParticleMaterial& Particle::material() {
    return *_material;
}

float Particle::timeStep() const {
    return std::any_cast<float>(shaderData.getData(_timeStepProp));
}

void Particle::setTimeStep(float step) {
    shaderData.setData(_timeStepProp, step);
}

std::shared_ptr<SampledTexture3D> Particle::vectorFieldTexture() const {
    return std::any_cast<std::shared_ptr<SampledTexture3D>>(shaderData.getData(_vectorFieldTextureProp));
}

void Particle::setVectorFieldTexture(const std::shared_ptr<SampledTexture3D>& field) {
    shaderData.setSampledTexure(_vectorFieldTextureProp, field);
}

Particle::SimulationVolume Particle::boundingVolumeType() const {
    return (Particle::SimulationVolume)std::any_cast<uint32_t>(shaderData.getData(_boundingVolumeProp));
}

void Particle::setBoundingVolumeType(SimulationVolume type) {
    shaderData.setData(_boundingVolumeProp, (uint32_t)type);
}

float Particle::bboxSize() const {
    return std::any_cast<float>(shaderData.getData(_bboxSizeProp));
}

void Particle::setBBoxSize(float size) {
    shaderData.setData(_bboxSizeProp, size);
}

float Particle::scatteringFactor() const {
    return std::any_cast<float>(shaderData.getData(_scatteringFactorProp));
}

void Particle::setScatteringFactor(float factor) {
    shaderData.setData(_scatteringFactorProp, factor);
}

float Particle::vectorFieldFactor() const {
    return std::any_cast<float>(shaderData.getData(_vectorFieldFactorProp));
}

void Particle::setVectorFieldFactor(float factor) {
    shaderData.setData(_vectorFieldFactorProp, factor);
}

float Particle::curlNoiseFactor() const {
    return std::any_cast<float>(shaderData.getData(_curlNoiseFactorProp));
}

void Particle::setCurlNoiseFactor(float factor) {
    shaderData.setData(_curlNoiseFactorProp, factor);
}

float Particle::curlNoiseScale() const {
    return std::any_cast<float>(shaderData.getData(_curlNoiseScaleProp));
}

void Particle::setCurlNoiseScale(float scale) {
    shaderData.setData(_curlNoiseScaleProp, scale);
}

float Particle::velocityFactor() const {
    return std::any_cast<float>(shaderData.getData(_velocityFactorProp));
}

void Particle::setVelocityFactor(float factor) {
    shaderData.setData(_velocityFactorProp, factor);
}

//MARK: - Emitter
uint32_t Particle::emitCount() const {
    return std::any_cast<uint32_t>(shaderData.getData(_emitCountProp));
}

void Particle::setEmitCount(uint32_t count) {
    shaderData.setData(_emitCountProp, count);
}

Particle::EmitterType Particle::emitterType() const {
    return (Particle::EmitterType)std::any_cast<uint32_t>(shaderData.getData(_emitterTypeProp));
}

void Particle::setEmitterType(EmitterType type) {
    shaderData.setData(_emitterTypeProp, (uint32_t)type);
}

Vector3F Particle::emitterPosition() const {
    return std::any_cast<Vector3F>(shaderData.getData(_emitterPositionProp));
}

void Particle::setEmitterPosition(const Vector3F& position) {
    shaderData.setData(_emitterPositionProp, position);
}

Vector3F Particle::emitterDirection() const {
    return std::any_cast<Vector3F>(shaderData.getData(_emitterDirectionProp));
}

void Particle::setEmitterDirection(const Vector3F& direction) {
    shaderData.setData(_emitterDirectionProp, direction);
}

float Particle::emitterRadius() const {
    return std::any_cast<float>(shaderData.getData(_emitterRadiusProp));
}

void Particle::setEmitterRadius(float radius) {
    shaderData.setData(_emitterRadiusProp, radius);
}

float Particle::particleMinAge() const {
    return std::any_cast<float>(shaderData.getData(_particleMinAgeProp));
}

void Particle::setParticleMinAge(float age) {
    shaderData.setData(_particleMinAgeProp, age);
}

float Particle::particleMaxAge() const {
    return std::any_cast<float>(shaderData.getData(_particleMaxAgeProp));
}

void Particle::setParticleMaxAge(float age) {
    shaderData.setData(_particleMaxAgeProp, age);
}

void Particle::_onEnable() {
    entity()->scene()->_particleManager.addParticle(this);
}

void Particle::_onDisable() {
    entity()->scene()->_particleManager.removeParticle(this);
}

}
