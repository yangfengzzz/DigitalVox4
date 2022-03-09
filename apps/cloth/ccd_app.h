//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef ccd_app_hpp
#define ccd_app_hpp

#include "cloth_application.h"

namespace vox {
namespace cloth {
class CCDApp : public ClothApplication {
public:
    void loadScene() override;
    
    void update(float delta_time) override;
    
private:
    void _initializeCloth(EntityPtr entity, physx::PxVec3 offset);
    
    nv::cloth::Fabric *_fabric{nullptr};
    nv::cloth::Solver *_solver{nullptr};
    ClothActor _clothActor;
    
    physx::PxVec3 _offset;
};

}
}
#endif /* ccd_app_hpp */
