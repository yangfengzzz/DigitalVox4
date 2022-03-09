//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef compute_hpp
#define compute_hpp

#include "forward_application.h"

namespace vox {
class AtomicComputeApp : public ForwardApplication {
public:
    bool prepare(Engine &engine) override;

    void loadScene() override;
    
private:
    std::shared_ptr<Material> _material{nullptr};
};
}

#endif /* compute_hpp */
