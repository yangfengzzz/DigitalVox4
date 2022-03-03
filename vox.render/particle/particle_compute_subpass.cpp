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
    NS::Error* error;
    // emission
    {
        _emissionCompute =
        CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                   _pass->library().newFunction(NS::String::string("particle_emission",
                                                                                   NS::StringEncoding::UTF8StringEncoding)));
        _emissionState = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState,
                                                    _context->device().newComputePipelineState(_emissionCompute.get(), &error));
        if (error != nullptr) {
            LOG(ERROR) << "Error: could not load Metal shader: "
            << error->description()->cString(NS::StringEncoding::UTF8StringEncoding) << std::endl;
        }
    }
    
    // simulation
    {
        _simulationCompute =
        CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                   _pass->library().newFunction(NS::String::string("particle_simulation",
                                                                                   NS::StringEncoding::UTF8StringEncoding)));
        _simulationState = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState,
                                                      _context->device().newComputePipelineState(_simulationCompute.get(), &error));
        if (error != nullptr) {
            LOG(ERROR) << "Error: could not load Metal shader: "
            << error->description()->cString(NS::StringEncoding::UTF8StringEncoding) << std::endl;
        }
    }
    
    // fillIndices
    {
        _fillIndicesCompute =
        CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                   _pass->library().newFunction(NS::String::string("particle_fill_indices",
                                                                                   NS::StringEncoding::UTF8StringEncoding)));
        _fillIndicesState = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState,
                                                       _context->device().newComputePipelineState(_fillIndicesCompute.get(), &error));
        if (error != nullptr) {
            LOG(ERROR) << "Error: could not load Metal shader: "
            << error->description()->cString(NS::StringEncoding::UTF8StringEncoding) << std::endl;
        }
    }
    
    // calculate dp
    {
        _calculateDPCompute =
        CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                   _pass->library().newFunction(NS::String::string("particle_calculate_dp",
                                                                                   NS::StringEncoding::UTF8StringEncoding)));
        _calculateDPState = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState,
                                                       _context->device().newComputePipelineState(_calculateDPCompute.get(), &error));
        if (error != nullptr) {
            LOG(ERROR) << "Error: could not load Metal shader: "
            << error->description()->cString(NS::StringEncoding::UTF8StringEncoding) << std::endl;
        }
    }
    
    // sort step
    {
        _sortStepCompute =
        CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                   _pass->library().newFunction(NS::String::string("particle_sort_step",
                                                                                   NS::StringEncoding::UTF8StringEncoding)));
        _sortStepState = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState,
                                                    _context->device().newComputePipelineState(_sortStepCompute.get(), &error));
        if (error != nullptr) {
            LOG(ERROR) << "Error: could not load Metal shader: "
            << error->description()->cString(NS::StringEncoding::UTF8StringEncoding) << std::endl;
        }
    }
    
    // sort final
    {
        _sortFinalCompute =
        CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                   _pass->library().newFunction(NS::String::string("particle_sort_final",
                                                                                   NS::StringEncoding::UTF8StringEncoding)));
        _sortFinalState = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState,
                                                     _context->device().newComputePipelineState(_sortFinalCompute.get(), &error));
        if (error != nullptr) {
            LOG(ERROR) << "Error: could not load Metal shader: "
            << error->description()->cString(NS::StringEncoding::UTF8StringEncoding) << std::endl;
        }
    }
}

void ParticleComputeSubpass::compute(MTL::ComputeCommandEncoder &commandEncoder) {
    auto particles = _scene->_particleManager.particles();
    for (auto& particle : particles) {
        _computeSingle(particle, commandEncoder);
    }
}

void ParticleComputeSubpass::_computeSingle(Particle* particle, MTL::ComputeCommandEncoder &commandEncoder) {
    commandEncoder.setBytes(&particle->timeStep(), sizeof(float), 0);
    
    commandEncoder.setComputePipelineState(_emissionState.get());
    commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    commandEncoder.setComputePipelineState(_simulationState.get());
    commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    commandEncoder.setComputePipelineState(_fillIndicesState.get());
    commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    commandEncoder.setComputePipelineState(_calculateDPState.get());
    commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    commandEncoder.setComputePipelineState(_sortStepState.get());
    commandEncoder.dispatchThreadgroups(MTL::Size(1, 1, 1), MTL::Size(1, 1, 1));
    commandEncoder.setComputePipelineState(_sortFinalState.get());
}


}
