//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef virtual_particle_app_hpp
#define virtual_particle_app_hpp

#include "cloth_application.h"

namespace vox {
namespace cloth {
class VirtualParticleApp : public ClothApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
    void update(float delta_time) override;
    
private:
    void _initializeCloth(EntityPtr entity, physx::PxVec3 offset);
    
    void _teleport();
    
    nv::cloth::Fabric *_fabric{nullptr};
    nv::cloth::Solver *_solver{nullptr};
    ClothActor _clothActor;
    
    physx::PxVec3 _offset;
};

}
}
#endif /* virtual_particle_app_hpp */
