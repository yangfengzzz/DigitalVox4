//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef free_fall_app_hpp
#define free_fall_app_hpp

#include "cloth_application.h"

namespace vox {
namespace cloth {
class FreeFallApp : public ClothApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
private:
    void _initializeCloth(EntityPtr entity, int index, physx::PxVec3 offset);
    
    nv::cloth::Fabric *_fabric[4];
    nv::cloth::Solver *_solver{nullptr};
    ClothActor _clothActor[4];
};

}
}
#endif /* free_fall_app_hpp */
