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

Particle::BoundingVolumeType Particle::boundingVolumeType() const {
    return _boundingVolume;
}

void Particle::setBoundingVolumeType(BoundingVolumeType type) {
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


}
