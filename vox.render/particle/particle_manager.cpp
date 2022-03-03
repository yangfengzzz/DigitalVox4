//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_manager.h"
#include <glog/logging.h>

namespace vox {
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

void ParticleManager::update(float deltaTime) {
    for (auto& particle : _particles) {
        particle->setTimeStep(deltaTime);
    }
}

}
