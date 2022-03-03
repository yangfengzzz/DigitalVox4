//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_compute_subpass.h"

namespace vox {
ParticleComputeSubpass::ParticleComputeSubpass(RenderContext *context,
                                               Scene *scene,
                                               Camera *camera):
ComputeSubpass(context, scene, camera){
    
}

void ParticleComputeSubpass::prepare() {
    
}

void ParticleComputeSubpass::compute(MTL::ComputeCommandEncoder &commandEncoder) {
    auto particles = _scene->_particleManager.particles();
    for (auto& particle : particles) {
        commandEncoder.setBytes(&particle->timeStep(), sizeof(float), 0);
    }
}

}
