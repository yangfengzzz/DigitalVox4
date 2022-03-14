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
    Shader::create("particle_point", "vertex_particle_point", "fragment_particle_point");
    Shader::create("particle_instancing", "vertex_particle_instancing", "fragment_particle_instancing");

    _emissionPass = std::make_unique<ComputePass>(library, scene, "particle_emission");
    _simulationPass = std::make_unique<ComputePass>(library, scene, "particle_simulation");
}

const std::vector<ParticleRenderer*>& ParticleManager::particles() const {
    return _particles;
}

void ParticleManager::addParticle(ParticleRenderer* particle) {
    auto iter = std::find(_particles.begin(), _particles.end(), particle);
    if (iter == _particles.end()) {
        _particles.push_back(particle);
    } else {
        LOG(ERROR) << "Particle already attached." << std::endl;;
    }
}

void ParticleManager::removeParticle(ParticleRenderer* particle) {
    auto iter = std::find(_particles.begin(), _particles.end(), particle);
    if (iter != _particles.end()) {
        _particles.erase(iter);
    }
}

float ParticleManager::timeStepFactor() const {
    return _timeStepFactor;
}

void ParticleManager::setTimeStepFactor(float factor) {
    _timeStepFactor = factor;
}

void ParticleManager::draw(MTL::CommandBuffer& commandBuffer) {
    auto encoder = CLONE_METAL_CUSTOM_DELETER(MTL::ComputeCommandEncoder,
                                              commandBuffer.computeCommandEncoder());
    for (auto& particle : _particles) {
        auto compileMacros = ShaderMacroCollection();
        particle->shaderData.mergeMacro(compileMacros, compileMacros);
        _computeSingle(particle, *encoder, compileMacros);
    }
    encoder->endEncoding();
}

void ParticleManager::_computeSingle(ParticleRenderer* particle,
                                     MTL::ComputeCommandEncoder &commandEncoder,
                                     const ShaderMacroCollection &compileMacros) {
    /* Max number of particles able to be spawned. */
    uint32_t const num_dead_particles = ParticleRenderer::kMaxParticleCount - particle->numAliveParticles();
    /* Number of particles to be emitted. */
    uint32_t const emit_count = std::min(ParticleRenderer::kBatchEmitCount, num_dead_particles); //
    _emission(emit_count, particle, commandEncoder, compileMacros);
    _simulation(emit_count, particle, commandEncoder, compileMacros);
}

void ParticleManager::_emission(const uint32_t count,
                                ParticleRenderer* particle, MTL::ComputeCommandEncoder &commandEncoder,
                                const ShaderMacroCollection &compileMacros) {
    /* Emit only if a minimum count is reached. */
    if (!count) {
        return;
    }
    if (count < ParticleRenderer::kBatchEmitCount) {
        //return;
    }
    
    particle->setEmitCount(count);
    _emissionPass->attachShaderData(&particle->shaderData);
    _emissionPass->setThreadsPerGrid(count);
    _emissionPass->detachShaderData(&particle->shaderData);
}

void ParticleManager::_simulation(const uint32_t count,
                                  ParticleRenderer* particle, MTL::ComputeCommandEncoder &commandEncoder,
                                  const ShaderMacroCollection &compileMacros) {
    if (particle->numAliveParticles() + count == 0u) {
        return;
    }
    
    _simulationPass->attachShaderData(&particle->shaderData);
    _simulationPass->setThreadsPerGrid(particle->numAliveParticles() + count);
    _simulationPass->detachShaderData(&particle->shaderData);
}


}
