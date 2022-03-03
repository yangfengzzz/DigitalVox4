//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle.h"
#include "entity.h"
#include "mesh/mesh_renderer.h"

namespace vox {
Particle::Particle(Entity *entity) :
Component(entity) {
    auto renderer = entity->addComponent<MeshRenderer>();
    _material = std::make_shared<ParticleMaterial>();
    renderer->setMaterial(_material);
}

ParticleMaterial& Particle::material() {
    return *_material;
}

float Particle::timeStep() const {
    return _timeStep;
}

void Particle::setTimeStep(float step) {
    _timeStep = step;
}

std::shared_ptr<SampledTexture3D> Particle::vectorFieldTexture() const {
    return _field;
}

void Particle::setVectorFieldTexture(const std::shared_ptr<SampledTexture3D>& field) {
    _field = field;
}

Particle::SimulationVolume Particle::boundingVolumeType() const {
    return _boundingVolume;
}

void Particle::setBoundingVolumeType(SimulationVolume type) {
    _boundingVolume = type;
}

float Particle::bboxSize() const {
    return _bboxSize;
}

void Particle::setBBoxSize(float size) {
    _bboxSize = size;
}

float Particle::scatteringFactor() const {
    return _scatteringFactor;
}

void Particle::setScatteringFactor(float factor) {
    _scatteringFactor = factor;
}

float Particle::vectorFieldFactor() const {
    return _vectorFieldFactor;
}

void Particle::setVectorFieldFactor(float factor) {
    _vectorFieldFactor = factor;
}

float Particle::curlNoiseFactor() const {
    return _curlNoiseFactor;
}

void Particle::setCurlNoiseFactor(float factor) {
    _curlNoiseFactor = factor;
}

float Particle::curlNoiseScale() const {
    return _curlNoiseScale;
}

void Particle::setCurlNoiseScale(float scale) {
    _curlNoiseScale = scale;
}

float Particle::velocityFactor() const {
    return _velocityFactor;
}

void Particle::setVelocityFactor(float factor) {
    _velocityFactor = factor;
}

bool Particle::enableScattering() const {
    return _enableScattering;
}

void Particle::setEnableScattering(bool flag) {
    _enableScattering = flag;
}

bool Particle::enableVectorField() const {
    return _enableVectorField;
}

void Particle::setEnableVectorField(bool flag) {
    _enableVectorField = flag;
}

bool Particle::enableCurlNoise() const {
    return _enableCurlNoise;
}

void Particle::setEnableCurlNoise(bool flag) {
    _enableCurlNoise = flag;
}

bool Particle::enableVelocityControl() const {
    return _enableVelocityControl;
}

void Particle::setEnableVelocityControl(bool flag) {
    _enableVelocityControl = flag;
}

//MARK: - Emitter
uint32_t Particle::emitCount() const {
    return _emitCount;
}

void Particle::setEmitCount(uint32_t count) {
    _emitCount = count;
}

Particle::EmitterType Particle::emitterType() const {
    return _emitterType;
}

void Particle::setEmitterType(EmitterType type) {
    _emitterType = type;
}

const Vector3F& Particle::emitterPosition() const {
    return _emitterPosition;
}

void Particle::setEmitterPosition(const Vector3F& position) {
    _emitterPosition = position;
}

const Vector3F& Particle::emitterDirection() const {
    return _emitterDirection;
}

void Particle::setEmitterDirection(const Vector3F& direction) {
    _emitterDirection = direction;
}

float Particle::emitterRadius() const {
    return _emitterRadius;
}

void Particle::setEmitterRadius(float radius) {
    _emitterRadius = radius;
}

float Particle::particleMinAge() const {
    return _particleMinAge;
}

void Particle::setParticleMinAge(float age) {
    _particleMinAge = age;
}

float Particle::particleMaxAge() const {
    return _particleMaxAge;
}

void Particle::setParticleMaxAge(float age) {
    _particleMaxAge = age;
}

}
