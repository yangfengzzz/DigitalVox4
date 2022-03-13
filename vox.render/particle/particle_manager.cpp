//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_manager.h"
#include "metal_helpers.h"
#include <glog/logging.h>

namespace vox {
ParticleManager::ParticleManager(MTL::Library &library, Scene *scene) {
    _emissionPass = std::make_unique<ComputePass>(library, scene, "particle_emission");
    _simulationPass = std::make_unique<ComputePass>(library, scene, "particle_simulation");
}

const std::vector<Particle*>& ParticleManager::particles() const {
    return _particles;
}

void ParticleManager::addParticle(Particle* particle) {
    auto iter = std::find(_particles.begin(), _particles.end(), particle);
    if (iter == _particles.end()) {
        _particles.push_back(particle);
    } else {
        LOG(ERROR) << "Particle already attached." << std::endl;;
    }
}

void ParticleManager::removeParticle(Particle* particle) {
    auto iter = std::find(_particles.begin(), _particles.end(), particle);
    if (iter != _particles.end()) {
        _particles.erase(iter);
    }
}

void ParticleManager::draw(MTL::CommandBuffer& commandBuffer) {
    auto encoder = CLONE_METAL_CUSTOM_DELETER(MTL::ComputeCommandEncoder,
                                              commandBuffer.computeCommandEncoder());
    _emissionPass->compute(*encoder);
    _simulationPass->compute(*encoder);
    encoder->endEncoding();
}

float ParticleManager::timeStepFactor() const {
    return _timeStepFactor;
}

void ParticleManager::setTimeStepFactor(float factor) {
    _timeStepFactor = factor;
}

}
