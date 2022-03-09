//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef tether_app_hpp
#define tether_app_hpp

#include "cloth_application.h"

namespace vox {
namespace cloth {
class TetherApp : public ClothApplication {
public:
    void loadScene() override;
    
private:
    void _initializeCloth(EntityPtr entity, int index, physx::PxVec3 offset, float tetherStiffness);
    
    nv::cloth::Fabric *_fabric[2];
    nv::cloth::Solver *_solver{nullptr};
    ClothActor _clothActor[2];
};

}
}
#endif /* tether_app_hpp */
