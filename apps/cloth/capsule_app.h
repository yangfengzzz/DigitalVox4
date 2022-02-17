//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef capsule_app_hpp
#define capsule_app_hpp

#include "forward_application.h"
#include <NvCloth/Solver.h>
#include <NvCloth/Fabric.h>

namespace vox {
namespace cloth {
class CapsuleApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
private:
    void _initializeCloth(physx::PxVec3 offset);
    
    nv::cloth::Fabric* _fabric{nullptr};
    std::unique_ptr<nv::cloth::Solver> _solver{nullptr};
};

}
}

#endif /* capsule_app_hpp */
