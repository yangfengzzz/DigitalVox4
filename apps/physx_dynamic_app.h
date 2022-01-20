//
//  physx_dynamic_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/20.
//

#ifndef physx_dynamic_app_hpp
#define physx_dynamic_app_hpp

#include "forward_application.h"
#include <random>

namespace vox {
class PhysXDynamicApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;

private:
    std::default_random_engine e;
    std::uniform_real_distribution<float> u;
};
 
}

#endif /* physx_dynamic_app_hpp */
