//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_manager_hpp
#define particle_manager_hpp

#include "particle.h"

namespace vox {
class ParticleManager {
public:
    ParticleManager() = default;
    
    void addParticle(Particle* particle);
    
    void removeParticle(Particle* particle);
    
    void update(float deltaTime);
    
private:
    std::vector<Particle*> _particles{nullptr};
};

}
#endif /* particle_manager_hpp */
