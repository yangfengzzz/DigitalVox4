//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_compute_subpass_hpp
#define particle_compute_subpass_hpp

#include "rendering/compute_subpass.h"

namespace vox {
class ParticleComputeSubpass : public ComputeSubpass {
public:
    ParticleComputeSubpass(RenderContext *context,
                           Scene *scene,
                           Camera *camera);
    
    void prepare() override;
    
    void compute(MTL::ComputeCommandEncoder &commandEncoder) override;

private:
    
};
}

#endif /* particle_compute_subpass_hpp */
