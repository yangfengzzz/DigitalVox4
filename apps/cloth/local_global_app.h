//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef local_global_app_hpp
#define local_global_app_hpp

#include "cloth_application.h"

namespace vox {
namespace cloth {
class LocalGlobalApp : public ClothApplication {
public:
    void loadScene() override;
    
    void update(float delta_time) override;
    
private:
    void _initializeCloth(EntityPtr entity, int index, physx::PxVec3 offset);
    
    nv::cloth::Fabric *_fabric[2];
    nv::cloth::Solver *_solver[2];
    ClothActor _clothActor[2];
    
    int _attachmentVertices[2];
    physx::PxVec4 _attachmentVertexOriginalPositions[2];

    float _time;
};

}
}
#endif /* local_global_app_hpp */
