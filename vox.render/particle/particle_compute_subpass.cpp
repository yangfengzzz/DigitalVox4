//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_compute_subpass.h"
#include "rendering/render_pass.h"
#include "metal_helpers.h"
#include <glog/logging.h>

namespace vox {
namespace {
uint32_t numTrailingBits(uint32_t const n) {
    uint32_t r = 0u;
    for (uint32_t i = n; i > 1u; i >>= 1u)
        ++r;
    return r;
}
} // namespace

ParticleComputeSubpass::ParticleComputeSubpass(RenderContext *context,
                                               Scene *scene,
                                               Camera *camera):
ComputeSubpass(context, scene, camera){
    
}

void ParticleComputeSubpass::prepare() {
    _pipelineDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineDescriptor, MTL::ComputePipelineDescriptor::alloc()->init());
}

void ParticleComputeSubpass::compute(MTL::ComputeCommandEncoder &commandEncoder) {
    auto particles = _scene->_particleManager.particles();
    for (auto& particle : particles) {
        auto compileMacros = ShaderMacroCollection();
        particle->shaderData.mergeMacro(compileMacros, compileMacros);
        _computeSingle(particle, commandEncoder, compileMacros);
    }
}

void ParticleComputeSubpass::_computeSingle(Particle* particle,
                                            MTL::ComputeCommandEncoder &commandEncoder,
                                            const ShaderMacroCollection &compileMacros) {
    /* Max number of particles able to be spawned. */
    uint32_t const num_dead_particles = Particle::kMaxParticleCount - particle->numAliveParticles();
    /* Number of particles to be emitted. */
    uint32_t const emit_count = std::min(Particle::kBatchEmitCount, num_dead_particles); //
    _emissionSingle(emit_count, particle, commandEncoder, compileMacros);
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_simulation", compileMacros);
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        uploadUniforms(commandEncoder, pipelineState->uniformBlock, particle->shaderData);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_fill_indices", compileMacros);
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        uploadUniforms(commandEncoder, pipelineState->uniformBlock, particle->shaderData);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_calculate_dp", compileMacros);
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        uploadUniforms(commandEncoder, pipelineState->uniformBlock, particle->shaderData);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_sort_step", compileMacros);
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        uploadUniforms(commandEncoder, pipelineState->uniformBlock, particle->shaderData);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_sort_final", compileMacros);
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        uploadUniforms(commandEncoder, pipelineState->uniformBlock, particle->shaderData);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
}

void ParticleComputeSubpass::_emissionSingle(const uint32_t count,
                                             Particle* particle, MTL::ComputeCommandEncoder &commandEncoder,
                                             const ShaderMacroCollection &compileMacros) {
    /* Emit only if a minimum count is reached. */
    if (!count) {
        return;
    }
    if (count < Particle::kBatchEmitCount) {
        //return;
    }
    
    auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_emission", compileMacros);
    _pipelineDescriptor->setComputeFunction(function);
    auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
    uploadUniforms(commandEncoder, pipelineState->uniformBlock, particle->shaderData);
    commandEncoder.setComputePipelineState(&pipelineState->handle());
    
    commandEncoder.setBuffer(particle->readAtomicBuffer().get(), 0, 7);
#if USE_SOA_LAYOUT
    commandEncoder.setBuffer(particle->readPositionBuffer().get(), 0, 8);
    commandEncoder.setBuffer(particle->readVelocityBuffer().get(), 0, 9);
    commandEncoder.setBuffer(particle->readAttributeBuffer().get(), 0, 10);
#else
    commandEncoder.setBuffer(particle->readAppendConsumeBuffer().get(), 0, 11);
#endif
    commandEncoder.setBuffer(particle->randomBuffer().get(), 0, 12);
    
    auto nWidth = pipelineState->handle().threadExecutionWidth();
    commandEncoder.dispatchThreads(MTL::Size(count, 1, 1), MTL::Size(nWidth, 1, 1));
}


}
