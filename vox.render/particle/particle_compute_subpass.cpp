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
        _computeSingle(particle, commandEncoder);
    }
}

void ParticleComputeSubpass::_computeSingle(Particle* particle, MTL::ComputeCommandEncoder &commandEncoder) {
    commandEncoder.setBytes(&particle->timeStep(), sizeof(float), 0);
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_emission", ShaderMacroCollection());
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_simulation", ShaderMacroCollection());
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_fill_indices", ShaderMacroCollection());
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_calculate_dp", ShaderMacroCollection());
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_sort_step", ShaderMacroCollection());
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
    
    {
        auto function = _pass->resourceCache().requestFunction(_pass->library(), "particle_sort_final", ShaderMacroCollection());
        _pipelineDescriptor->setComputeFunction(function);
        auto pipelineState = _pass->resourceCache().requestPipelineState(*_pipelineDescriptor);
        commandEncoder.setComputePipelineState(&pipelineState->handle());
        commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    }
}


}
