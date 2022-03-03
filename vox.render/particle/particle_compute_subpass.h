//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_compute_subpass_hpp
#define particle_compute_subpass_hpp

#include "rendering/compute_subpass.h"

namespace vox {
class Particle;

class ParticleComputeSubpass : public ComputeSubpass {
public:
    ParticleComputeSubpass(RenderContext *context,
                           Scene *scene,
                           Camera *camera);
    
    void prepare() override;
    
    void compute(MTL::ComputeCommandEncoder &commandEncoder) override;

private:
    void _computeSingle(Particle* particle, MTL::ComputeCommandEncoder &commandEncoder);
    
    std::shared_ptr<MTL::Function> _emissionCompute{nullptr};
    std::shared_ptr<MTL::ComputePipelineState> _emissionState{nullptr};
    
    std::shared_ptr<MTL::Function> _simulationCompute{nullptr};
    std::shared_ptr<MTL::ComputePipelineState> _simulationState{nullptr};
    
    std::shared_ptr<MTL::Function> _fillIndicesCompute{nullptr};
    std::shared_ptr<MTL::ComputePipelineState> _fillIndicesState{nullptr};
    
    std::shared_ptr<MTL::Function> _calculateDPCompute{nullptr};
    std::shared_ptr<MTL::ComputePipelineState> _calculateDPState{nullptr};
    
    std::shared_ptr<MTL::Function> _sortStepCompute{nullptr};
    std::shared_ptr<MTL::ComputePipelineState> _sortStepState{nullptr};
    
    std::shared_ptr<MTL::Function> _sortFinalCompute{nullptr};
    std::shared_ptr<MTL::ComputePipelineState> _sortFinalState{nullptr};
};
}

#endif /* particle_compute_subpass_hpp */
