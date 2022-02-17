//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cloth_application_hpp
#define cloth_application_hpp

#include "forward_application.h"
#include <NvCloth/Solver.h>
#include <NvCloth/Fabric.h>
#include <NvCloth/Cloth.h>

namespace vox {
namespace cloth {
class ClothApplication: public ForwardApplication {
public:
    struct ClothActor {
        Renderer* clothRenderable{nullptr};
        nv::cloth::Cloth* cloth{nullptr};
    };
    
    bool prepare(Engine &engine) override;
    
    void update(float delta_time) override;
    
protected:
    nv::cloth::Factory* _factory{nullptr};
};

}
}
#endif /* cloth_application_hpp */
