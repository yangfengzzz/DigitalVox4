//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_hpp
#define particle_hpp

#include "component.h"
#include "particle_material.h"

namespace vox {
class Particle : public Component {
public:
    explicit Particle(Entity *entity);
    
    ParticleMaterial& material();
    
public:
    
    
private:
    std::shared_ptr<ParticleMaterial> _material{nullptr};
};
}

#endif /* particle_hpp */
