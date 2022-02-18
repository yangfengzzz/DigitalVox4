//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef teleport_app_hpp
#define teleport_app_hpp

#include "cloth_application.h"

namespace vox {
namespace cloth {
class TeleportApp : public ClothApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
    void update(float delta_time) override;
    
private:
    void _initializeCloth(EntityPtr entity, physx::PxVec3 offset);
    
    void _teleport();
    
    nv::cloth::Fabric *_fabric{nullptr};
    nv::cloth::Solver *_solver{nullptr};
    ClothActor _clothActor;
    
    float _time;
};

}
}
#endif /* teleport_app_hpp */
