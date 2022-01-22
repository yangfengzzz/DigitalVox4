//
//  skybox_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/22.
//

#ifndef skybox_app_hpp
#define skybox_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class SkyboxApp : public ForwardApplication {
public:
    bool prepare(Engine &engine) override;

    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* skybox_app_hpp */
