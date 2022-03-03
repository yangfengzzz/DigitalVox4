//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle.h"
#include "entity.h"
#include "mesh/mesh_renderer.h"

namespace vox {
Particle::Particle(Entity *entity) :
Component(entity) {
    auto renderer = entity->addComponent<MeshRenderer>();
    _material = std::make_shared<ParticleMaterial>();
    renderer->setMaterial(_material);
}

}
