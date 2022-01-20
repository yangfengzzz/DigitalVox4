//
//  physx_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/20.
//

#ifndef physx_app_hpp
#define physx_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class PhysXApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
};
 
}

#endif /* physx_app_hpp */
