//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef compute_hpp
#define compute_hpp

#include "forward_application.h"
#include "rendering/compute_pass.h"

namespace vox {
class AtomicComputeApp : public ForwardApplication {
public:
    bool prepare(Engine &engine) override;

    void loadScene() override;
    
    void updateGPUTask(MTL::CommandBuffer& commandBuffer) override;
    
private:
    std::shared_ptr<Material> _material{nullptr};
    std::unique_ptr<ComputePass> _pass{nullptr};
};
}

#endif /* compute_hpp */
