//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wind_app_hpp
#define wind_app_hpp

#include "cloth_application.h"

namespace vox {
namespace cloth {
class WindApp : public ClothApplication {
public:
    void loadScene() override;
    
    void update(float delta_time) override;
    
private:
    void _initializeCloth(EntityPtr entity, int index, physx::PxVec3 offset);
    
    nv::cloth::Fabric *_fabric[3];
    nv::cloth::Solver *_solver{nullptr};
    ClothActor _clothActor[3];
    
    float _time;
};

}
}
#endif /* wind_app_hpp */
