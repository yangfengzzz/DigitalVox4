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
    
    const std::vector<Particle*>& particles() const;
    
    void addParticle(Particle* particle);
    
    void removeParticle(Particle* particle);
    
    void update(float deltaTime);
    
public:
    float timeStepFactor() const;

    void setTimeStepFactor(float factor);
    
private:
    std::vector<Particle*> _particles{nullptr};
    float _timeStepFactor = 1.0f;
};

}
#endif /* particle_manager_hpp */
